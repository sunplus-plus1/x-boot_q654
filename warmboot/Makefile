.PHONY: all

OUT = warmboot

all: clean
	$(MAKE) -C xboot
	@cp xboot/xboot ./$(OUT)

.PHONY: clean
clean:

	$(MAKE) -C xboot clean

	$(MAKE) -C a64up clean
