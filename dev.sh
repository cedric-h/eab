cd build
find ../src | \
    grep -E "[^.swp]$" | \
    entr -rs 'make && cd eugenics-auto-battler && ./eugenics-auto-battler'
