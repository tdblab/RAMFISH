# RAMFISH vs bulk RNA-seq vs single-cell RNA-seq
# Compares relative abundance of each gene between methods (Spearman and Pearson on log10 values)
# Needs: ggplot2, ggrepel, patchwork
#   install.packages(c("ggplot2", "ggrepel", "patchwork"))

setwd("K:/methods_RAMFISH/v31_MEE/supp_figure/spearman correlation/R")

library(ggplot2)
library(ggrepel)
library(patchwork)

# ---- settings ----------------------------------------------------------------
forewing_file <- "Forewing_xy_values.csv"
hindwing_file <- "hindwing_xy_values.csv"

# gene labels shown on the plots (left = name in the CSV, right = name on the plot)
rename_genes <- c(sal = "spalt", rho3 = "rho", AntP = "Antp")

base_size  <- 20    # font size for axes and titles
label_size <- 5.5   # font size for gene labels
point_col  <- "#2F5D8A"
line_col   <- "#B03A2E"

# ---- read data and compute relative abundance (count / total x 100,000) ------
ra <- function(x) x / sum(x) * 1e5

fw <- read.csv(forewing_file, check.names = FALSE)
fw <- data.frame(gene     = fw$Gene,
                 RAMFISH  = ra(fw$RAMFISH_puncta),
                 Bulk     = ra(fw$Bulk_mean_count))

hw <- read.csv(hindwing_file, check.names = FALSE)
names(hw)[1] <- "Gene"
hw <- data.frame(gene     = hw$Gene,
                 RAMFISH  = ra(hw$RAMFISH_spots),
                 Bulk     = ra(hw$bulk_mean_count),
                 scRNA    = ra(hw$sc_total_count))

relabel <- function(g) ifelse(g %in% names(rename_genes), rename_genes[g], g)
fw$gene <- relabel(fw$gene)
hw$gene <- relabel(hw$gene)

# ---- statistics --------------------------------------------------------------
axis_names <- c(RAMFISH = "RAMFISH", Bulk = "Bulk RNA-seq", scRNA = "scRNA-seq")

compare <- function(d, x, y, tissue) {
  # exact P when there are no tied values, otherwise the standard approximation
  s <- suppressWarnings(cor.test(d[[x]], d[[y]], method = "spearman"))
  p <- cor.test(log10(d[[x]]), log10(d[[y]]), method = "pearson")
  data.frame(tissue = tissue,
             comparison = paste(axis_names[y], "vs", axis_names[x]),
             n_genes = nrow(d),
             spearman_rho = unname(s$estimate),
             spearman_p = s$p.value,
             pearson_r_log10 = unname(p$estimate),
             pearson_p = p$p.value)
}

stats <- rbind(compare(fw, "Bulk",  "RAMFISH", "Forewing"),
               compare(hw, "Bulk",  "RAMFISH", "Hindwing"),
               compare(hw, "scRNA", "RAMFISH", "Hindwing"),
               compare(hw, "scRNA", "Bulk",    "Hindwing"))
stats_out <- transform(stats,
                       spearman_rho = round(spearman_rho, 3), spearman_p = signif(spearman_p, 3),
                       pearson_r_log10 = round(pearson_r_log10, 3), pearson_p = signif(pearson_p, 3))
print(stats_out)
write.csv(stats_out, "correlation_statistics.csv", row.names = FALSE)

# ---- plotting ----------------------------------------------------------------
fmt_p <- function(p) if (p < 0.001) "P < 0.001" else sprintf("P = %.3f", p)

# axis ticks at 1, 10, 100 ...; add 2 and 5 in between when the axis spans few decades
log_breaks <- function(lims) {
  m <- if (log10(lims[2] / lims[1]) < 2.5) c(1, 2, 5) else 1
  b <- as.vector(outer(m, 10^(-1:6)))
  b[b >= lims[1] & b <= lims[2]]
}

scatter <- function(d, x, y, st) {
  lab <- sprintf("Spearman \u03C1 = %.2f (%s)\nPearson r (log values) = %.2f, n = %d genes",
                 st$spearman_rho, fmt_p(st$spearman_p), st$pearson_r_log10, st$n_genes)
  ggplot(d, aes(.data[[x]], .data[[y]])) +
    geom_smooth(method = "lm", formula = y ~ x, se = TRUE,
                colour = line_col, fill = "grey80", linewidth = 0.9, linetype = "dashed") +
    geom_point(shape = 21, size = 3.6, fill = point_col, colour = "white", stroke = 0.6) +
    geom_text_repel(aes(label = gene), fontface = "italic", size = label_size,
                    colour = "grey15", segment.colour = "grey60", segment.size = 0.3,
                    box.padding = 0.35, point.padding = 0.25, min.segment.length = 0.2,
                    max.overlaps = Inf, seed = 1) +
    scale_x_log10(breaks = log_breaks, labels = scales::comma, expand = expansion(mult = 0.08)) +
    scale_y_log10(breaks = log_breaks, labels = scales::comma, expand = expansion(mult = 0.08)) +
    annotation_logticks(sides = "bl", colour = "grey40",
                        short = unit(0.08, "cm"), mid = unit(0.14, "cm"), long = unit(0.2, "cm")) +
    labs(x = paste0(axis_names[x], "\n(relative abundance per 100,000)"),
         y = paste0(axis_names[y], "\n(relative abundance per 100,000)"),
         subtitle = lab) +
    theme_classic(base_size = base_size) +
    theme(aspect.ratio = 1,
          axis.text = element_text(colour = "black"),
          axis.line = element_line(linewidth = 0.6),
          axis.ticks = element_line(linewidth = 0.6),
          plot.tag = element_text(face = "bold", size = base_size + 6),
          plot.title = element_text(face = "bold", hjust = 0.5),
          plot.subtitle = element_text(hjust = 0.5, size = base_size - 2, lineheight = 1.1))
}

save_fig <- function(p, name, w, h) {
  ggsave(paste0(name, ".pdf"), p, width = w, height = h, device = cairo_pdf)
  ggsave(paste0(name, ".png"), p, width = w, height = h, dpi = 300, bg = "white")
  ggsave(paste0(name, ".tiff"), p, width = w, height = h, dpi = 300, bg = "white",
         compression = "lzw")
}

# Forewing: RAMFISH vs bulk
p_fw <- scatter(fw, "Bulk", "RAMFISH", stats[1, ]) + ggtitle("Forewing")
save_fig(p_fw, "Forewing_RAMFISH_vs_bulk", 8, 8)

# Hindwing: (A) RAMFISH vs bulk, (B) RAMFISH vs scRNA-seq, (C) bulk vs scRNA-seq
pA <- scatter(hw, "Bulk",  "RAMFISH", stats[2, ])
pB <- scatter(hw, "scRNA", "RAMFISH", stats[3, ])
pC <- scatter(hw, "scRNA", "Bulk",    stats[4, ])
p_hw <- (pA | pB | pC) + plot_annotation(tag_levels = "A")
save_fig(p_hw, "Hindwing_RAMFISH_bulk_scRNA", 24, 8.5)

# each hindwing panel on its own, in case you want to arrange them yourself
save_fig(pA, "Hindwing_A_RAMFISH_vs_bulk", 8, 8)
save_fig(pB, "Hindwing_B_RAMFISH_vs_scRNA", 8, 8)
save_fig(pC, "Hindwing_C_bulk_vs_scRNA", 8, 8)

cat("Done. Figures and correlation_statistics.csv saved in", getwd(), "\n")
