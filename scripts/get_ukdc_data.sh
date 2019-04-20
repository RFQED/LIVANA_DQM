
# Example:
# source scripts/get_ukdc_data.sh /data/lz/MDC2 background/LZAP-3.1.0-PHYSICS-3.1.0/20170402


outdir=$1
files=$2
lscmd="xrdfs gfe02.grid.hep.ph.ic.ac.uk ls"
cpcmd="xrdcp"

copy() {
    pat=$1
    outdir=$2

    for f in $($lscmd $pat)
      do
      xrdcp root://gfe02.grid.hep.ph.ic.ac.uk/$f $outdir/.
    done
}

copy_mdc2() {
    indir=$1
    outdir=$2    
    basedir="/pnfs/hep.ph.ic.ac.uk/data/lz/lz/data/MDC2"
    mkdir $outdir/$indir
    copy $basedir/$indir $outdir/$indir
}


copy_mdc2 $files $outdir
