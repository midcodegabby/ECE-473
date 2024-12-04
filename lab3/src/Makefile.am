noinst_PROGRAMS = main

# Source files for the program
main_SOURCES = main.c lcd/lcd.c lcd/glyph.c

install-exec-local:
	avrdude -c "$(PROGRAMMER)" -p "$(MCU)" -P "$(PROGRAMMER_PORT)" -e -U flash:w:main -U eeprom:w:main
