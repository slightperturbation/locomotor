data.hgData = read.table( "harnessGen.data", header=T )
data.hgData  # print the data

aov.hgData = aov( bestFit~harnessGens, data=data.hgData )

summary( aov.hgData )
print( model.tables( aov.hgData, "means"), digits=3 )  #
boxplot( bestFit~harnessGens, data=data.hgData )

