# Ray Tracer

**Disciplina:** Computação Gráfica  
**Instituição:** Universidade Federal de Minas Gerais (UFMG)  
**Professor:** Renato Ferreira
**Aluno:** Pedro Dalla-Lana - 2020420613

---

## Descrição

Implementação completa de um renderizador por **Ray Tracing** em C++17 que processa cenas descritas em arquivos texto e gera imagens no formato PPM. O programa implementa iluminação global com reflexões, refrações, sombras e múltiplos tipos de superfícies.

---

## Funcionalidades Implementadas

### Funcionalidade Básica

#### **1. Câmera Configurável**
- Posição da câmera (eye)
- Ponto de foco (lookAt)
- Orientação (up vector)
- Campo de visão vertical (fovy)
- Resolução configurável via linha de comando (padrão: 800×600)

#### **2. Sistema de Iluminação**
- **Luz Ambiente:** Primeira luz do arquivo (iluminação base da cena)
- **Luzes Pontuais:** Múltiplas fontes de luz com:
  - Posição no espaço 3D
  - Cor RGB
  - Atenuação (constante, linear, quadrática)
- **Modelo de Phong:**
  - Componente ambiente (ka)
  - Componente difusa (kd)
  - Componente especular (ks)
  - Expoente especular (alpha)

#### **3. Sombras**
- Shadow rays para cada fonte de luz
- Oclusão correta entre objetos
- Múltiplas sombras (várias fontes)

#### **4. Reflexão e Refração**
- **Reflexão:** Coeficiente kr (raios refletidos recursivos)
- **Refração:** Coeficiente kt com Lei de Snell
- **Índice de Refração:** ior configurável
- Profundidade máxima de recursão: 5 níveis

#### **5. Pigmentos (Texturas)**
- **Solid:** Cor sólida RGB
- **Checker:** Padrão xadrez 3D procedural com escala configurável
- **Texmap:** Mapeamento de textura 2D de arquivos PPM
  - Transformação de coordenadas 3D → 2D via matrizes P0 e P1
  - Repetição automática (wrapping)

#### **6. Primitivas Geométricas**
- **Esfera:** Centro e raio
- **Poliedro Convexo:** Definido por interseção de semi-espaços (planos)
- **Triângulo:** Algoritmo de Möller-Trumbore
- **Cilindro:** Base, eixo, altura e raio
- **Cone:** Ápice, eixo, altura e raio da base
- **Quádrica Geral:** Superfícies de segundo grau (Ax² + By² + Cz² + ...)

---

### Funcionalidades Extras Implementadas

#### **Ray Tracing Distribuído**
1. **Anti-Aliasing**
   - Jittering: Múltiplas amostras por pixel com perturbação aleatória
   - Configurável via linha de comando (padrão: 16 amostras)
   - Reduz serrilhamento (aliasing) nas bordas

2. **Depth of Field**
   - Simulação de abertura de câmera (aperture)
   - Distância focal configurável (focus distance)
   - Efeito de desfoque em objetos fora do foco
   - Ativa via parâmetros de linha de comando

#### **Novas Superfícies**
- **Triângulo:** Interseção eficiente com Möller-Trumbore
- **Cilindro:** Superfície cilíndrica finita
- **Cone:** Superfície cônica finita
- **Quádrica:** Elipsoides, paraboloides, hiperboloides, etc.

---

## Arquitetura do Código

### Estrutura de Diretórios
```
projeto/
├── include/              # Headers (.hpp)
│   ├── vec3.hpp         # Classe de vetores 3D
│   ├── scene.hpp        # Estruturas de dados (Scene, Object, Light, etc.)
│   ├── intersect.hpp    # Funções de interseção raio-objeto
│   ├── pigment.hpp      # Sistema de pigmentos/texturas
│   ├── shading.hpp      # Modelo de iluminação (Phong + recursivo)
│   ├── loader.hpp       # Carregamento de arquivos de cena
│   └── raytracer.hpp    # Classe principal do renderizador
├── src/                 # Implementações (.cpp)
│   ├── scene.cpp
│   ├── intersect.cpp
│   ├── pigment.cpp
│   ├── shading.cpp
│   ├── loader.cpp
│   ├── raytracer.cpp
│   └── main.cpp
├── testes/              # Arquivos de cena (.in)
│   ├── test1.in
│   ├── test2.in
│   ├── test3.in
│   ├── test4.in
│   └── test5.in
├── resultados/          # Imagens geradas (.ppm)
├── Makefile            # Sistema de build
└── README.md           # Este arquivo
```

### Módulos Principais

#### **1. vec3.hpp/cpp**
- Classe `Vec3` para vetores 3D
- Operações: soma, subtração, multiplicação, produto escalar, produto vetorial
- Métodos: normalização, reflexão, clamp

#### **2. scene.hpp/cpp**
- Estruturas de dados da cena:
  - `Ray`: Raio com origem e direção
  - `Scene`: Contém câmera, luzes, objetos, pigmentos e acabamentos
  - `Object`: Representa objetos geométricos
  - `Light`: Fonte de luz
  - `Pigment`: Sistema de cores/texturas
  - `Finish`: Propriedades de material (ka, kd, ks, alpha, kr, kt, ior)
  - `HitInfo`: Informação de interseção

#### **3. intersect.hpp/cpp**
- Namespace `Intersect` com funções especializadas:
  - `sphere()`: Interseção raio-esfera
  - `polyhedron()`: Interseção raio-poliedro convexo
  - `triangle()`: Interseção raio-triângulo (Möller-Trumbore)
  - `cylinder()`: Interseção raio-cilindro
  - `cone()`: Interseção raio-cone
  - `quadric()`: Interseção raio-quádrica
- Funções auxiliares:
  - `solveQuadratic()`: Resolve equações quadráticas (reutilizável)
  - `adjustNormal()`: Garante normal apontando contra o raio

#### **4. pigment.hpp/cpp**
- `getPigmentColor()`: Calcula cor do pigmento em um ponto 3D
  - Solid: Retorna cor constante
  - Checker: Padrão xadrez 3D baseado em coordenadas
  - Texmap: Mapeia coordenadas 3D → 2D e busca cor na textura

#### **5. shading.hpp/cpp**
- Modelo de iluminação completo:
  - `traceRay()`: Função principal de ray tracing (recursiva)
  - `shade()`: Calcula cor final de um ponto
  - Funções auxiliares:
    - `calculateAmbient()`: Componente ambiente
    - `calculateDiffuse()`: Componente difusa
    - `calculateSpecular()`: Componente especular (Phong)
    - `calculateReflection()`: Raios refletidos recursivos
    - `calculateRefraction()`: Raios refratados recursivos (Lei de Snell)
    - `isInShadow()`: Testa se ponto está em sombra

#### **6. loader.hpp/cpp**
- `loadScene()`: Carrega arquivo de cena completo
- `loadPPM()`: Carrega texturas em formato PPM (P3 ASCII e P6 binário)
- Parsing robusto com tratamento de comentários

#### **7. raytracer.hpp/cpp**
- Classe `RayTracer`: Gerencia renderização
  - `loadScene()`: Carrega cena de arquivo
  - `render()`: Loop principal de renderização
  - `savePPM()`: Salva imagem em formato PPM P3
  - Suporte a anti-aliasing (múltiplas amostras)
  - Suporte a depth of field (abertura e foco)

#### **8. main.cpp**
- Interface de linha de comando
- Parsing de argumentos
- Inicialização do sistema

---

## Compilação

### Requisitos
- **Compilador:** g++ com suporte a C++17
- **Sistema:** Linux, macOS ou Windows (com MinGW/MSYS2)

### Comandos

```bash
# Compilar (otimizado)
make

# Compilar com debug
make debug

# Limpar objetos
make clean

# Limpar tudo (objetos + executável + imagens)
make distclean
```

### Flags de Compilação
- `-std=c++17`: C++17 (para std::clamp, etc.)
- `-O3`: Otimização máxima
- `-march=native`: Otimizações específicas do processador
- `-Wall -Wextra`: Avisos detalhados

---

## Execução

### Sintaxe
```bash
./bin/ray_tracer <arquivo_entrada.in> <arquivo_saida.ppm> [largura] [altura] [amostras] [abertura] [dist_focal]
```

### Exemplos

```bash
# Renderização básica (800x600, 16 amostras)
./bin/ray_tracer testes/test1.in resultados/test1.ppm

# Resolução customizada
./bin/ray_tracer testes/test5.in resultados/test5.ppm 1920 1080

# Com anti-aliasing forte (64 amostras)
./bin/ray_tracer testes/test4.in resultados/test4.ppm 800 600 64

# Com Depth of Field (abertura=0.5, foco=15)
./bin/ray_tracer testes/test4.in resultados/test4_dof.ppm 800 600 32 0.5 15

# Teste rápido (baixa resolução, poucas amostras)
./bin/ray_tracer testes/test3.in resultados/test3_quick.ppm 400 300 4
```

---

## Testes

### Atalhos do Makefile

```bash
# Testes padrão (800x600, 16 amostras)
make test1    # Teste básico com esferas
make test2    # Múltiplas luzes
make test3    # Padrão checker
make test4    # Reflexão e refração
make test5    # Cena completa (arquivo do enunciado)

make test     # Executar tests 1-5

# Testes rápidos (400x300, 4 amostras)
make quick-test1
make quick-test2
make quick-test3
make quick-test4
make quick-test5

make tests-quick  # Todos os testes rápidos

# Testes HD (1920x1080, 64 amostras)
make hd-test4
make hd-test5

# Testes Depth of Field
make dof-test4    # Test4 com DOF
make dof-test5    # Test5 com DOF
```

### Descrição dos Testes

#### **test1.in** - Básico
- 4 esferas coloridas
- 3 fontes de luz
- Teste de iluminação Phong
- Teste de sombras

#### **test2.in** - Múltiplas Luzes
- 5 esferas
- 3 luzes pontuais
- Uma esfera gigante como fundo
- Teste de atenuação

#### **test3.in** - Texturas Procedurais
- 1 esfera
- 1 plano (poliedro)
- Pigmento checker (xadrez)
- 4 fontes de luz coloridas

#### **test4.in** - Reflexão e Refração
- 3 esferas com materiais diferentes
- Poliedros como chão
- Teste de kr (reflexão)
- Teste de kt (transmissão)
- Textura PPM (blur.ppm)

#### **test5.in** - Cena Completa (do Enunciado)
- 11 esferas em arranjo
- 1 poliedro (caixa)
- Textura rainbow (texmap)
- Padrão checker
- Reflexão forte (kr=0.7)
- Iluminação complexa

---

## Formato do Arquivo de Entrada

O arquivo `.in` segue a especificação do enunciado com 5 seções:

### 1. Câmera (4 linhas)
```
eye_x eye_y eye_z
lookAt_x lookAt_y lookAt_z
up_x up_y up_z
fovy
```

### 2. Luzes
```
num_luzes
pos_x pos_y pos_z  cor_r cor_g cor_b  atten_const atten_linear atten_quad
...
```
**Nota:** A primeira luz é sempre ambiente.

### 3. Pigmentos
```
num_pigmentos
solid r g b
checker r1 g1 b1  r2 g2 b2  scale
texmap arquivo.ppm  p0[0] p0[1] p0[2] p0[3]  p1[0] p1[1] p1[2] p1[3]
...
```

### 4. Acabamentos (Finishes)
```
num_acabamentos
ka kd ks alpha kr kt ior
...
```

### 5. Objetos
```
num_objetos
pigment_idx finish_idx sphere center_x center_y center_z radius
pigment_idx finish_idx polyhedron num_faces
    a b c d
    ...
pigment_idx finish_idx triangle v0_x v0_y v0_z  v1_x v1_y v1_z  v2_x v2_y v2_z
pigment_idx finish_idx cylinder base_x base_y base_z  axis_x axis_y axis_z  height radius
pigment_idx finish_idx cone apex_x apex_y apex_z  axis_x axis_y axis_z  height radius
pigment_idx finish_idx quadric A B C D E F G H I J
...
```

---

## Formato de Saída

**Formato:** PPM P3 (ASCII)

```
P3
largura altura
255
r g b  r g b  r g b  ...
```

- Cores em [0, 255]
- Um pixel por trio de valores
- Compatível com visualizadores padrão (GIMP, ImageMagick, etc.)

### Visualizar Imagens

```bash
# ImageMagick
display resultados/test5.ppm

# GIMP
gimp resultados/test4.ppm

# Converter para PNG
convert resultados/test5.ppm resultados/test5.png
```

---

## Otimizações Implementadas

### Algoritmos Eficientes
1. **Möller-Trumbore** para triângulos (sem pré-computação de plano)
2. **Resolução quadrática unificada** (`solveQuadratic()`)
3. **Template para cilindro/cone** (zero overhead)
4. **Lookup table** para tipos de objetos (elimina switch-case)

### C++ Moderno
- `std::clamp()` para clamping
- Range-based loops
- `static_cast` explícito
- `constexpr` para constantes
- Referências constantes (`const&`)
- Move semantics onde apropriado

### Compilação
- `-O3`: Otimização agressiva
- `-march=native`: Instruções específicas da CPU (AVX, SSE, etc.)
- Inlining automático de funções pequenas

### Memória
- `reserve()` em vetores (evita realocações)
- Objetos passados por referência
- Minimização de cópias

---

## Performance Esperada

| Resolução | Amostras | Objetos | Tempo Estimado |
|-----------|----------|---------|----------------|
| 400×300 | 4 | 5-10 | ~1-3 segundos |
| 800×600 | 16 | 5-10 | ~10-30 segundos |
| 1920×1080 | 64 | 10-15 | ~5-15 minutos |

**Nota:** Tempos variam com CPU, número de reflexões/refrações e complexidade da cena.

---

## Debugging

### Build Debug
```bash
make debug
gdb ./bin/ray_tracer
```

### Testes Rápidos
Use resolução baixa e poucas amostras:
```bash
./bin/ray_tracer testes/test4.in debug.ppm 200 150 1
```


## Autor
Pedro Dalla-Lana
Trabalho Prático RT-1  
Disciplina: Computação Gráfica  
UFMG - Departamento de Ciência da Computação

---

## Notas de Implementação

### Interpretações do Enunciado

1. **Reflexão/Refração:** Cores são **somadas** à cor local (não multiplicadas)
   ```
   cor_final = cor_local + kr × cor_refletida + kt × cor_transmitida
   ```

2. **Especular:** Usa cor da luz, **não** cor do objeto (Phong tradicional)

3. **Luz Ambiente:** Primeira luz é sempre ambiente (coordenadas são ignoradas)

4. **Normais:** Sempre ajustadas para apontar contra o raio

5. **Cor de Fundo:** Cinza escuro (0.1, 0.1, 0.1)

### Limitações
- Não implementa CSG (Constructive Solid Geometry)
- Não implementa motion blur (temporal)
- Texturas apenas em formato PPM
- Não implementa BVH/KD-tree (aceleração espacial)