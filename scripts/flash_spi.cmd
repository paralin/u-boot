setenv blink_work 'led work on; sleep 0.1; led work off; sleep 0.1'
setenv blink_diy 'led diy on; sleep 0.1; led diy off; sleep 0.1'

run blink_work
if sf probe; then
  run blink_work

  if size ${devtype} ${devnum}:${distro_bootpart} spi_combined.img; then
    run blink_work
    load ${devtype} ${devnum}:${distro_bootpart} ${kernel_addr_r} spi_combined.img
  
    # write flash
    run blink_work
    run blink_work
    run blink_work
    sf update ${kernel_addr_r} 0 ${filesize}
    run blink_work
    run blink_work
    run blink_work
  
    led work on
    echo "Wrote U-Boot to SPI Flash successfully."
    while true; do sleep 1; done
  else
    echo "Error: Missing spi_combined.img"
    # blink red LED forever
    while true; do run blink_diy; sleep 0.1; done
  fi
else
  echo "Error: No SPI flash."
  # blink both LEDs forever
  while true; do run blink_diy; run blink_work; done
fi
