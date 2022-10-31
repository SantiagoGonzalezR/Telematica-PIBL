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

## Desarrollo
La creacion del codigo fue un proceso extenso, el cual tuvo mucho ensayo y error, inicialmente el proposito iba a ser realizar el proyecto en python, pero las limitaciones impuestas sobre y el acceso a las librerias de C de linux culmino en nuestra decision de trabajar en C, con el uso de algunos tutoriales y la documentacion de sockets en C logramos que inicialmente se hiciera una conexion en un entorno privado de consola.

Despues de cierto tiempo y un poco de investigacion, descubrimos como lograr una conexion tipo HTTP1.1 haciendo peticiones desde un browser a la ip del proxy, con este conocimiento, realizamos las conexiones a los 3 servidores y aplicamos el cache y el log que nos fueron pedidos.

## Conclusiones
-Trabajar con C aunque puede resultar mas dificil que otros lenguajes operativos es un desafio interesante y genera soluciones mas eficientes que con otros lenguajes.

-La implementacion de un balanceador de carga es sencilla dependiendo de la estrategia, en este caso, round robbin

-La implementacion del proxy es extensa, pero una vez implementado, implementar los servidores se convierte en una tarea mucho mas sencilla.

## Referencias
https://www.youtube.com/watch?v=Pg_4Jz8ZIH4&t=189s

https://www.geeksforgeeks.org/socket-programming-cc/
