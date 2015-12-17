#to run this you have to have the following libraries installed
# . fmcsR
# . ChemmineR
# . ChemmineOB (need openbabel)

library("fmcsR")
smiset <- read.SMIset("../docs/smiles.smi")
sm<-smiles2sdf(smiset)
test <- fmcs(sm[1], sm[2])
plotMCS(test)
