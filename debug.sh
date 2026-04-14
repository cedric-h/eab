cd src

make clean && \
    make BUILD_MODE=DEBUG && \
    lldb eugenics_auto_battler
