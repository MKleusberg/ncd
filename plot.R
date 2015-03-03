args <- commandArgs(trailingOnly=TRUE)

ncd.rawdata <- read.table(args[1], header=TRUE, row.names=1)
ncd.dist <- as.dist(ncd.rawdata, diag=TRUE, upper=FALSE)
h <- hclust(ncd.dist, method="mcquitty")
prob.ticks = (1:10)/10
prob.subticks = prob.ticks + 0.05
prob.subsubticks = (0:100)/100
plot(h, ylab="NCD", xlab="Set", axes=FALSE)
axis(2, at=prob.subsubticks, labels=FALSE, tck=-0.005)
axis(2, at=prob.subticks, labels=FALSE, tck=-0.01)
axis(2, at=prob.ticks, labels=TRUE, tck=-0.02)
