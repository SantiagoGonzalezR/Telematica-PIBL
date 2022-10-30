# Telematica-PIBL
Proyecto 1 de aula, realizado por Jose Manuel Fonseca, Santiago Gonzalez y Mariana Vasquez

## Descripcion del proyecto
Proxy inverso con balanceador de carga, conectado a 3 servidores y recibe peticiones de un navegador web a partir de la IP publica de el proxy. El proxy se inicia por consola con el comando **./proxy n**, tiene un archivo de configuracion (config.txt) donde se ponen las IPs de los 3 servidores a conectar y el puerto al cual se va a acceder; el parametro que este debe recibir (n) es el tiempo de vida de cache, en nuestro caso, hacemos un cache **FIFO** (First in First out). Los servidores tambien se despliegan desde consola con el comando **./servern port**, donde se especifica cual servidor es (n) y su parametro (port) es el puerto por el cual reciben y envian informacion.

El proxy va generando un log de la actividad que se realiza sobre este, especificamente, la IP del servidor al cual se va a conectar y su respuesta, o, en caso que esta conexion falle, la respuesta del cache.

## Prerrequisitos para correr el codigo
Para correr el codigo se necesitan instalar el compilador gcc y pthread en la maquina en la que se va a correr el codigo, esto se puede hacer con el siguiente comando

`sudo apt install gcc \
                 libpthread-stubs0-dev`

Para que este pueda correr bien, se debe modificar el archivo **config.txt** que esta junto al codigo **proxy.c** con la informacion pertinente, dado que, si esto no se cambia, el codigo intentara hacer conexiones con IPs que no existen o son ajenas al funcionamiento del sistema.
Se deben correr el proxy y los servidores para que el sistema funcione adecuadamente, despues de esto se podra realizar la conexion al proxy usando la url **http://ipproxy:puerto**, cabe notar que la IP que se debe usar es la publica, no la privada.
