# HFP
The Heat Flow Program is a one‑dimensional finite element simulation designed to compute temperature development through layered materials exposed to fire.


# 1D Transient Heat Conduction Solver

Denna solver simulerar **1D transient värmeledning** med temperatur som primär variabel. Den använder en nodbaserad explicit metod istället för traditionella matrisoperationer, vilket gör den enkel, snabb och minnes-effektiv.

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

Randvillkor (strålning + konvektion) hanteras per nod:

\[
\dot{Q}_i = \varepsilon \sigma (T_r^4 - T_i^4) + h(T_g - T_i)
\]

---

## 2. Diskretisering

### 2.1 Element och noder

Materialet delas in i \(n_\text{elem}\) linjära element med noder \(i = 0,1,...,n\). Varje element \(e\) kopplar två noder \(i\) och \(j=i+1\).  

- Noderna lagrar temperaturer \(T_i\) vid varje tidssteg.  
- Elementets längd är \(\Delta x_e\).

### 2.2 Lumpad kapacitetsmatris

Istället för att bygga en full matris används **lumpad kapacitans** per nod:

\[
C_i = \sum_{\text{element } e \ni i} \frac{\rho_e c_e \Delta x_e}{2}
\]

Detta motsvarar den diagonala kapacitetsmatrisen i klassisk FEM:

\[
\overline{\mathbf{C}} = 
\begin{bmatrix}
C_0 & 0 & \dots & 0 \\
0 & C_1 & \dots & 0 \\
\vdots & \vdots & \ddots & \vdots \\
0 & 0 & \dots & C_n
\end{bmatrix}
\]

---

### 2.3 Konduktivitetsbidrag

För varje element \(e\) beräknas ledningsflödet mellan nod \(i\) och \(j\):

\[
q_{i\to j} = \frac{k_e}{\Delta x_e} (T_i - T_j)
\]

Detta är exakt samma som att multiplicera elementets konduktivitetsmatris med temperaturvektorn:

\[
K^e = \frac{k_e}{\Delta x_e} 
\begin{bmatrix} 1 & -1 \\ -1 & 1 \end{bmatrix}
\]

Globalt ackumuleras bidragen till nodernas effektvektor:

\[
Q_i = \sum_{\text{grannar}} q_{i \to j} + Q_i^\text{boundary}
\]

---

## 3. Explicit tidssteg

Solvern använder **explicit Euler** för tidsintegration:

\[
T_i^{n+1} = T_i^n + \Delta t \frac{Q_i}{C_i}
\]

- \(C_i\) är nodens lumpade kapacitet  
- \(Q_i\) är summan av ledningsflöden och randvillkor  

Detta motsvarar den diskreta FEM-formeln:

\[
\bar{\mathbf{T}}^{j+1} = \bar{\mathbf{T}}^j + \Delta t \, \overline{\mathbf{C}}^{-1} \left[ \bar{\mathbf{Q}}^j - \mathbf{K} \bar{\mathbf{T}}^j \right]
\]

Skillnaden är att vi **direkt beräknar varje nods uppdatering utan att skapa stora matriser**, vilket ger samma resultat.

---

## 4. Randvillkor

Exponerad sida (eld):

\[
Q_0^\text{boundary} = \varepsilon \sigma (T_\text{fire}^4 - T_0^4) + h_\text{exposed} (T_\text{fire} - T_0)
\]

Oexponerad sida (ambient):

\[
Q_n^\text{boundary} = \varepsilon \sigma (T_\text{ambient}^4 - T_n^4) + h_\text{ambient} (T_\text{ambient} - T_n)
\]

---

## 5. Varför detta ger samma resultat som traditionell FEM

1. **Lumpad kapacitans** motsvarar diagonalelementen i \(C\)-matrisen i standard FEM.  
2. **Elementledningsflöden** beräknas exakt som \(\mathbf{K}\mathbf{T}\).  
3. **Explicit Euler** ger samma tidsdiskretisering som \(\bar{\mathbf{T}}^{j+1} = \bar{\mathbf{T}}^j + \Delta t C^{-1}(\bar{Q} - K \bar{T})\).  
4. **Boundary conditions** adderas direkt till nodvektorn, vilket är ekvivalent med FEM:s globalvektor.  

> Slutsats: nodbaserad explicit implementation är algebraiskt identisk med standard FEM, men sparar minne och undviker onödiga matrisoperationer.

---

## 6. Sammanfattning

- Solvern är **1D lumpad FEM med explicit tidssteg**.  
- Beräkning sker **direkt per nod**, vilket undviker fulla matrismultiplikationer.  
- Resultaten är **matematiskt ekvivalenta** med traditionell FEM.

