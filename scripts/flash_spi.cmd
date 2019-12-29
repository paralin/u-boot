setenv blink_work 'led work on; sleep 0.1; led work off'
setenv blink_diy 'led diy on; sleep 0.1; led diy off'

run blink_work
sf probe
run blink_work

if size ${devtype} ${devnum}:${distro_bootpart} spi_combined.img; then
  run blink_work
  load ${devtype} ${devnum}:${distro_bootpart} ${kernel_addr_r} spi_combined.img

  # write flash
  run blink_work
  sleep 0.1
  run blink_work
  sleep 0.1
  run blink_work
  sleep 0.1
  sf update ${kernel_addr_r} 0 ${filesize}

  led work on
  while true; do sleep 1; done
else
  echo "missing spi_combined.img"
  # blink red LED forever
  while true; do run blink_diy; sleep 0.1; done
fi
