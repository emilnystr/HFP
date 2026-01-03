# 1D Transient Heat Conduction Solver

Denna solver simulerar **1D transient värmeledning** med temperatur som primär variabel. Den använder en nodbaserad explicit metod istället för traditionella matrisoperationer.

---

## 1. Grundekvation

Värmeledningsekvationen i 1D:

\[
\rho(T) c(T) \frac{\partial T}{\partial t} = \frac{\partial}{\partial x} \left( k(T) \frac{\partial T}{\partial x} \right)
\]

där:

- \(T(x,t)\) = temperatur [°C]  
- \(k(T)\) = värmeledningsförmåga [W/m·K]  
- \(\rho(T)\) = densitet [kg/m³]  
- \(c(T)\) = specifik värmekapacitet [J/kg·K]  

Randvillkor (strålning + konvektion) per nod:

\[
\dot{Q}_i = \varepsilon \sigma \left(T_r^4 - T_i^4\right) + h \left(T_g - T_i\right)
\]

---

## 2. Diskretisering

### 2.1 Noder och element

Mesh med \(n_\text{elem}\) linjära element, noder \(i = 0,1,...,n\).  
Element \(e\) kopplar noder \(i\) och \(j=i+1\) med längd \(\Delta x_e\).

---

### 2.2 Lumpad kapacitetsmatris

Lumpad kapacitans per nod:

\[
C_i = \sum_{\text{element } e \ni i} \frac{\rho_e c_e \Delta x_e}{2}
\]

Motsvarar diagonal kapacitetsmatris i FEM:

\[
\mathbf{C} =
\begin{bmatrix}
C_0 & 0 & \dots & 0 \\
0 & C_1 & \dots & 0 \\
\vdots & \vdots & \ddots & \vdots \\
0 & 0 & \dots & C_n
\end{bmatrix}
\]

---

### 2.3 Konduktivitetsbidrag

För element \(e\) med noder \(i,j\):

\[
q_{i\to j} = \frac{k_e}{\Delta x_e} \left( T_i - T_j \right)
\]

Global nodvektor:

\[
Q_i = \sum_{\text{grannar } j} q_{i \to j} + Q_i^\text{boundary}
\]

Detta motsvarar matrisformeln:

\[
\mathbf{K}^e =
\frac{k_e}{\Delta x_e} 
\begin{bmatrix} 1 & -1 \\ -1 & 1 \end{bmatrix}, \quad
\mathbf{Q} = \mathbf{Q} - \mathbf{K} \mathbf{T}
\]

---

## 3. Explicit tidssteg

Temperaturen uppdateras nod för nod med **explicit Euler**:

\[
T_i^{n+1} = T_i^n + \Delta t \frac{Q_i}{C_i}
\]

Motsvarar den diskreta FEM-formeln:

\[
\mathbf{T}^{j+1} = \mathbf{T}^{j} + \Delta t \, \mathbf{C}^{-1} (\mathbf{Q}^j - \mathbf{K} \mathbf{T}^j)
\]

---

## 4. Randvillkor

Exponerad sida (eld):

\[
Q_0^\text{boundary} = \varepsilon \sigma \left(T_\text{fire}^4 - T_0^4\right) + h_\text{exposed} \left(T_\text{fire} - T_0\right)
\]

Oexponerad sida (ambient):

\[
Q_n^\text{boundary} = \varepsilon \sigma \left(T_\text{ambient}^4 - T_n^4\right) + h_\text{ambient} \left(T_\text{ambient} - T_n\right)
\]

---

## 5. Ekvivalens med traditionell FEM

1. Lumpad kapacitans \(C_i\) ↔ diagonalelement i \(\mathbf{C}\)  
2. Ledningsflöden \(q_{i\to j}\) ↔ elementets \(\mathbf{K}^e \mathbf{T}\)  
3. Explicit tidssteg ↔ \(\mathbf{T}^{j+1} = \mathbf{T}^j + \Delta t \mathbf{C}^{-1} (\mathbf{Q}^j - \mathbf{K} \mathbf{T}^j)\)  
4. Boundary conditions adderas direkt till nodvektorn  

> Slutsats: Nodbasserad explicit implementation ger **exakt samma algebraiska resultat** som traditionell FEM, men utan att skapa fulla matriser.

---

## 6. Sammanfattning

- **1D lumpad FEM** med explicit tidssteg  
- Beräkning sker **direkt per nod**, sparar minne och undviker onödiga matrisoperationer  
- Resultaten är **matematiskt identiska** med standard FEM
