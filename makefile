# Makefile - Ray Tracer RT-1 (Refatorado)

CXX = g++
CXXFLAGS = -std=c++17 -O3 -march=native -Wall -Wextra -I./include
LDFLAGS = -lm

# Diretórios
SRCDIR = src
INCDIR = include
BINDIR = bin
OBJDIR = obj
RESDIR = resultados
TESTDIR = testes

# Target
TARGET = $(BINDIR)/ray_tracer

# Lista completa de arquivos fonte
SOURCES = $(SRCDIR)/main.cpp \
          $(SRCDIR)/raytracer.cpp \
          $(SRCDIR)/scene.cpp \
          $(SRCDIR)/intersect.cpp \
          $(SRCDIR)/pigment.cpp \
          $(SRCDIR)/shading.cpp \
          $(SRCDIR)/loader.cpp

# Objetos correspondentes
OBJECTS = $(OBJDIR)/main.o \
          $(OBJDIR)/raytracer.o \
          $(OBJDIR)/scene.o \
          $(OBJDIR)/intersect.o \
          $(OBJDIR)/pigment.o \
          $(OBJDIR)/shading.o \
          $(OBJDIR)/loader.o

# Headers
HEADERS = $(INCDIR)/vec3.hpp \
          $(INCDIR)/scene.hpp \
          $(INCDIR)/intersect.hpp \
          $(INCDIR)/pigment.hpp \
          $(INCDIR)/shading.hpp \
          $(INCDIR)/loader.hpp \
          $(INCDIR)/raytracer.hpp

# Regra principal
all: $(TARGET)

# Criar diretórios
$(BINDIR) $(OBJDIR) $(RESDIR):
	@mkdir -p $@

# Linkar executável
$(TARGET): $(OBJECTS) | $(BINDIR)
	@echo "Linkando $(TARGET)..."
	@$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build concluído!"

# Compilar main.cpp
$(OBJDIR)/main.o: $(SRCDIR)/main.cpp $(INCDIR)/raytracer.hpp | $(OBJDIR)
	@echo "Compilando main.cpp..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar raytracer.cpp
$(OBJDIR)/raytracer.o: $(SRCDIR)/raytracer.cpp $(INCDIR)/raytracer.hpp $(INCDIR)/shading.hpp $(INCDIR)/loader.hpp | $(OBJDIR)
	@echo "Compilando raytracer.cpp..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar scene.cpp
$(OBJDIR)/scene.o: $(SRCDIR)/scene.cpp $(INCDIR)/scene.hpp $(INCDIR)/vec3.hpp | $(OBJDIR)
	@echo "Compilando scene.cpp..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar intersect.cpp
$(OBJDIR)/intersect.o: $(SRCDIR)/intersect.cpp $(INCDIR)/intersect.hpp $(INCDIR)/scene.hpp | $(OBJDIR)
	@echo "Compilando intersect.cpp..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar pigment.cpp
$(OBJDIR)/pigment.o: $(SRCDIR)/pigment.cpp $(INCDIR)/pigment.hpp $(INCDIR)/scene.hpp | $(OBJDIR)
	@echo "Compilando pigment.cpp..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar shading.cpp
$(OBJDIR)/shading.o: $(SRCDIR)/shading.cpp $(INCDIR)/shading.hpp $(INCDIR)/intersect.hpp $(INCDIR)/pigment.hpp | $(OBJDIR)
	@echo "Compilando shading.cpp..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar loader.cpp
$(OBJDIR)/loader.o: $(SRCDIR)/loader.cpp $(INCDIR)/loader.hpp $(INCDIR)/scene.hpp | $(OBJDIR)
	@echo "Compilando loader.cpp..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpeza
clean:
	@echo "Removendo objetos..."
	@rm -rf $(OBJDIR)

distclean: clean
	@echo "Removendo executável e resultados..."
	@rm -rf $(BINDIR) $(RESDIR)/*.ppm

# Build debug
debug: CXXFLAGS = -std=c++17 -g -O0 -Wall -Wextra -I./include
debug: clean all
	@echo "Build debug concluído!"

# Testes individuais
test1: $(TARGET) | $(RESDIR)
	@echo "Executando test1..."
	@./$(TARGET) $(TESTDIR)/test1.in $(RESDIR)/test1.ppm

test2: $(TARGET) | $(RESDIR)
	@echo "Executando test2..."
	@./$(TARGET) $(TESTDIR)/test2.in $(RESDIR)/test2.ppm

test3: $(TARGET) | $(RESDIR)
	@echo "Executando test3..."
	@./$(TARGET) $(TESTDIR)/test3.in $(RESDIR)/test3.ppm

test4: $(TARGET) | $(RESDIR)
	@echo "Executando test4..."
	@./$(TARGET) $(TESTDIR)/test4.in $(RESDIR)/test4.ppm

test5: $(TARGET) | $(RESDIR)
	@echo "Executando test5..."
	@./$(TARGET) $(TESTDIR)/test5.in $(RESDIR)/test5.ppm

test6: $(TARGET) | $(RESDIR)
	@echo "Executando test6..."
	@./$(TARGET) $(TESTDIR)/test6.in $(RESDIR)/test6.ppm

testL: $(TARGET) | $(RESDIR)
	@echo "Executando test6..."
	@./$(TARGET) $(TESTDIR)/testL.in $(RESDIR)/testL.ppm

# Testes rápidos (resolução reduzida)
quick-test1: $(TARGET) | $(RESDIR)
	@echo "Teste rápido test1..."
	@./$(TARGET) $(TESTDIR)/test1.in $(RESDIR)/test1-quick.ppm 400 300 4

quick-test2: $(TARGET) | $(RESDIR)
	@echo "Teste rápido test2..."
	@./$(TARGET) $(TESTDIR)/test2.in $(RESDIR)/test2-quick.ppm 400 300 4

quick-test3: $(TARGET) | $(RESDIR)
	@echo "Teste rápido test3..."
	@./$(TARGET) $(TESTDIR)/test3.in $(RESDIR)/test3-quick.ppm 400 300 4

quick-test4: $(TARGET) | $(RESDIR)
	@echo "Teste rápido test4..."
	@./$(TARGET) $(TESTDIR)/test4.in $(RESDIR)/test4-quick.ppm 400 300 4

quick-test5: $(TARGET) | $(RESDIR)
	@echo "Teste rápido test5..."
	@./$(TARGET) $(TESTDIR)/test5.in $(RESDIR)/test5-quick.ppm 400 300 4

quick-test6: $(TARGET) | $(RESDIR)
	@echo "Teste rápido test6..."
	@./$(TARGET) $(TESTDIR)/test6.in $(RESDIR)/test6-quick.ppm 400 300 4

quick-testL: $(TARGET) | $(RESDIR)
	@echo "Teste rápido test6..."
	@./$(TARGET) $(TESTDIR)/testL.in $(RESDIR)/testL-quick.ppm 400 300 4

# HD tests (resolução alta)
hd-test4: $(TARGET) | $(RESDIR)
	@echo "Teste HD test4 (1920x1080, 64 samples)..."
	@./$(TARGET) $(TESTDIR)/test4.in $(RESDIR)/test4-hd.ppm 1920 1080 64

hd-test5: $(TARGET) | $(RESDIR)
	@echo "Teste HD test5 (1920x1080, 64 samples)..."
	@./$(TARGET) $(TESTDIR)/test5.in $(RESDIR)/test5-hd.ppm 1920 1080 64

# Depth of Field tests
dof-test4: $(TARGET) | $(RESDIR)
	@echo "Teste DOF test4 (abertura=0.5, foco=15)..."
	@./$(TARGET) $(TESTDIR)/test4.in $(RESDIR)/test4-dof.ppm 800 600 32 0.5 15

dof-test5: $(TARGET) | $(RESDIR)
	@echo "Teste DOF test5 (abertura=1.0, foco=200)..."
	@./$(TARGET) $(TESTDIR)/test5.in $(RESDIR)/test5-dof.ppm 800 600 32 1.0 200

# Executar todos os testes
test: test1 test2 test3 test4 test5 test6 testL
	@echo "Todos os testes padrão concluídos!"

tests-quick: quick-test1 quick-test2 quick-test3 quick-test4 quick-test5 quick-test6
	@echo "Todos os testes rápidos concluídos!"

tests-all: test
	@if [ -f $(TESTDIR)/test6.in ]; then $(MAKE) test6; fi
	@echo "Todos os testes disponíveis concluídos!"

# Help
help:
	@echo "Ray Tracer RT-1 - Sistema de Build"
	@echo ""
	@echo "Uso: make [target]"
	@echo ""
	@echo "Targets principais:"
	@echo "  all (default) - Compilar o programa"
	@echo "  clean         - Remover objetos"
	@echo "  distclean     - Remover objetos, executável e resultados"
	@echo "  debug         - Compilar em modo debug"
	@echo ""
	@echo "Testes:"
	@echo "  test          - Executar testes padrão (test1-5)"
	@echo "  test1-6       - Executar teste individual"
	@echo "  tests-quick   - Testes rápidos (baixa resolução)"
	@echo "  tests-all     - Todos os testes disponíveis"
	@echo ""
	@echo "Testes avançados:"
	@echo "  quick-test1-6 - Teste rápido individual"
	@echo "  hd-test4-5    - Testes em alta resolução"
	@echo "  dof-test4-5   - Testes com Depth of Field"
	@echo ""
	@echo "Para mais informações, consulte README.md"

.PHONY: all clean distclean debug test test1 test2 test3 test4 test5 test6 \
        quick-test1 quick-test2 quick-test3 quick-test4 quick-test5 quick-test6 \
        hd-test4 hd-test5 dof-test4 dof-test5 \
        tests-quick tests-all help