PRIV_DIR := $(MIX_APP_PATH)/priv
NIF_PATH := $(PRIV_DIR)/libquickjs_nif.so
C_SRC := $(shell pwd)/c_src

CPPFLAGS := -shared -fPIC -fvisibility=hidden -std=c++20 -Wall -Wextra
CPPFLAGS += -I$(ERTS_INCLUDE_DIR) -I$(FINE_INCLUDE_DIR) -I$(QUICKJS_INCLUDE_DIR) $(QUICKJS_LIB_DIR)/libquickjs.a
ifdef DEBUG
	CPPFLAGS += -g
else
	CPPFLAGS += -O3
endif

ifndef TARGET_ABI
  TARGET_ABI := $(shell uname -s | tr '[:upper:]' '[:lower:]')
endif

ifeq ($(TARGET_ABI),darwin)
	CPPFLAGS += -undefined dynamic_lookup -flat_namespace
endif

SOURCES := $(wildcard $(C_SRC)/*.cpp)

all: $(NIF_PATH)
	@ echo > /dev/null # Dummy command to avoid the default output "Nothing to be done"

$(NIF_PATH): $(SOURCES) vendor/quickjs/build
	@ mkdir -p $(PRIV_DIR)
	$(CXX) $(CPPFLAGS) $(SOURCES) -o $(NIF_PATH)

vendor/quickjs/build:
	@echo "Building QuickJS..."
	cd vendor/quickjs && $(MAKE)
	mkdir -p vendor/quickjs/build/include vendor/quickjs/build/lib
	cp vendor/quickjs/quickjs.h vendor/quickjs/build/include/
	cp vendor/quickjs/libquickjs.a vendor/quickjs/build/lib/
