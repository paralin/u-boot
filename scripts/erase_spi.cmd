setenv blink_work 'led work on; sleep 0.1; led work off; sleep 0.1'
setenv blink_diy 'led diy on; sleep 0.1; led diy off; sleep 0.1'

echo "Enable SPI Flash now."

run blink_work
sleep 2
run blink_work
sleep 2
run blink_work
sleep 2
run blink_work
sleep 2
run blink_work
sleep 2

if sf probe; then

  # erase all mtd partitions containing u-boot
  run blink_work
  run blink_work
  run blink_work
  mtd erase u-boot-spl
  run blink_work
  run blink_work
  run blink_work
  mtd erase u-boot
  run blink_work
  run blink_work
  run blink_work
  mtd erase u-boot-env

  led work on
  echo "Erased U-Boot from SPI Flash."
  while true; do sleep 1; done
else
  echo "Error: No SPI flash."
  # blink both LEDs forever
  while true; do run blink_diy; run blink_work; done
fi
