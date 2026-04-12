find ./src | \
    grep -E "[^.swp|.o]$" | \
    entr -rs 'cd src && make && ./eugenics_auto_battler'
