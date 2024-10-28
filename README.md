# ZX-ESPectrum-Final-MicroP

> **IMPORTANTE**: Este proyecto es una adaptación de [dcrespo3d](https://github.com/dcrespo3d/ZX-ESPectrum-Wiimote) para el proyecto final de la asignatura de Microprocesadores.

## Descripción del Proyecto
Este proyecto es un emulador para el procesador Z80 en una placa ESP32, utilizando un teclado PS/2 y una pantalla VGA para simular el entorno de la computadora Sinclair ZX Spectrum. A continuación se detallan los principales hitos del desarrollo.

## Roles y Herramientas Utilizadas
- **Roles**:
  - Andrea Arias: Product Manager
  - Orlando Arroyo: Hardware Developer
  - Karen Barros y Cristian Vasco: Software Developers

- **Herramientas**:
  - Notion para la asignación de tareas y seguimiento
  - Visual Studio Code, Arduino IDE
  - GitHub para control de versiones

---

## Proceso de Desarrollo

### Conexiones de Hardware
1. **Adaptación del conector VGA**:
   - Se soldaron los pines del conector VGA a la placa ESP32, asegurando la continuidad de las conexiones.
   - ![Diagrama de conexiones VGA](docs\img\pinesvga.png)
   - ![Adaptación VGA](docs\img\vgaapadtado.png)

2. **Conector PS/2**:
   - Adaptación de un conector PS/2 Hembra mediante un convertidor PS/2 a USB.
   - ![Diagrama de conexiones PS/2](docs\img\ps2pines.png)
   - ![Adaptación PS/2](docs\img\ps2adaptado.png)

3. **Conversor de Niveles Lógicos**:
   - Implementado para asegurar una correcta comunicación entre el ESP32 y el teclado PS/2.
   
### Pruebas de Hardware
- **Pruebas de pantalla con VGA**:
  - Verificación del logo en colores básicos y ajustes en el código para mostrar diferentes colores.
  - ![Pruebas de Pantalla](docs\img\pruebacolores.png)

- **Prueba del Teclado PS/2**:
  - Se evaluó la comunicación del teclado con la ESP32 y se solucionaron problemas de compatibilidad de alimentación.

---

## Integración de Software
1. **Emulador de Z80**:
   - Código ajustado para emular juegos y colores específicos en el monitor VGA.
   

2. **Conexión Wii**:
   - Se probó la conexión con un control de Wii mediante Bluetooth. Aunque exitosa, no se integró en la versión final.

---

## Fase Final
- **Empaquetado**:
  - Caja diseñada para proteger el circuito y mejorar la estética del dispositivo.
  - ![Empaquetado](docs\img\empaquetado.png)

- **Prueba y Presentación Final**:
  - Montaje y prueba final del sistema en condiciones de uso para asegurar su correcto funcionamiento.

---

## Resultados
El proyecto cumplió con los objetivos establecidos, implementando un emulador funcional del ZX Spectrum en la ESP32 y logrando una interacción con el hardware y software planificados.

> **Nota**: Consulta el [README original de dcrespo3d](https://github.com/dcrespo3d/ZX-ESPectrum-Wiimote) para más detalles técnicos.
