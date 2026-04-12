cd build
find src | entr -rs 'make && cd eugenics-auto-battler && ./eugenics-auto-battler && cd ..'
