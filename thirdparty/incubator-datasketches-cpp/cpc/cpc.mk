# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

CPC_SRCDIR := cpc/src
CPC_INCDIR := cpc/include
CPC_BUILDDIR := build/cpc

CPC_TSTDIR := cpc/test
CPC_TSTBUILD := cpc/build

CPC_TEST_BIN := cpc_test
CPC_TARGET := cpc/$(CPC_TEST_BIN)

CPC_CSOURCES := $(shell find $(CPC_SRCDIR) -type f -name "*.c")
CPC_CPPSOURCES := $(shell find $(CPC_SRCDIR) -type f -name "*.cpp")
CPC_SOURCES := $(CPC_CSOURCES) $(CPC_CPPSOURCES)
CPC_OBJECTS := $(patsubst $(CPC_SRCDIR)/%,$(CPC_BUILDDIR)/%,$(CPC_CSOURCES:.c=.o) $(CPC_CPPSOURCES:.cpp=.o))

CPC_TSTSOURCES := $(shell find $(CPC_TSTDIR) -type f -name "*.cpp")
CPC_TSTOBJS := $(patsubst $(CPC_TSTDIR)/%,$(CPC_TSTBUILD)/%,$(CPC_TSTSOURCES:.cpp=.o))

CPC_INCLIST := $(COM_INCLIST) $(patsubst $(CPC_SRCDIR)/%,-I $(CPC_SRCDIR)/%,$(CPC_INCDIR))
CPC_INCLIST := $(COM_INCLIST) -I $(CPC_INCDIR)
CPC_BUILDLIST := $(patsubst src/%,$(CPC_BUILDDIR)/%,$(CPC_INCDIR))

cpc: $(CPC_OBJECTS)

$(CPC_BUILDDIR)/%.o: $(CPC_SRCDIR)/%.cpp
	@mkdir -p $(CPC_BUILDDIR)
	@echo "Compiling $<...";
	@$(CC) $(CPPFLAGS) $(INC) $(CPC_INCLIST) -c -o $@ $<

$(CPC_BUILDDIR)/%.o: $(CPC_SRCDIR)/%.c
	@mkdir -p $(CPC_BUILDDIR)
	@echo "Compiling $<...";
	@$(CC) $(CFLAGS) $(INC) $(CPC_INCLIST) -c -o $@ $<

$(CPC_TSTBUILD)/%.o: $(CPC_TSTDIR)/%.cpp
	@mkdir -p $(CPC_TSTBUILD)
	@echo "Compiling $<...";
	@$(CC) $(CPPFLAGS) $(INC) $(CPC_INCLIST) -c -o $@ $<

.PHONY: cpc_test cpc_clean
cpc_exec: $(COM_TSTOBJS) $(CPC_OBJECTS) $(CPC_TSTOBJS)
	@echo "Linking $(CPC_TARGET)..."
	@$(CC) $^ -o $(CPC_TARGET) $(TSTLNKFLAGS) $(LIB)

cpc_test: $(LIBRARY) cpc_exec
	@cd cpc; LD_LIBRARY_PATH=../$(TARGETDIR) ./$(CPC_TEST_BIN)

cpc_clean:
	@echo "Cleaning cpc...";
	@$(RM) -r $(CPC_BUILDDIR) $(CPC_TSTBUILD) $(CPC_TARGET)
