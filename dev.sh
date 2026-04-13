find ./src | \
    grep -E "[^.swp|.o|.d]$" | \
    entr -rs 'cd src && make && (./eugenics_auto_battler)'
