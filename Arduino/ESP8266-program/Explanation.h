/* 
 *  # ESP8266-program
 *  
 *  De ESP8266 is een microcontroller, net zoals een Arduino. Het grootste verschil is dat de ESP vele malen sneller is dan de Arduino UNO, veel meer geheugen heeft, en een Wi-Fi transceiver heeft.
 *  De ESP kan over een seriele UART connectie communiceren met de Arduino. Er wordt gebruik gemaakt van een eenvoudig binair serieel protocol.
 *  Op de ESP gebeuren verschillende dingen:
 *  - Als de constante STATION gedefinieerd is, wordt geprobeerd verbinding te maken met een Wi-Fi access point (AP). (Een aparaat dat verbonden is met een AP noemt men een station).
 *  - De ESP is zelf ook een Wi-Fi AP, zo kan je ermee verbinden om nieuwe data te uploaden, of om te communiceren met het wagentje zonder dat een extern AP nodig is.
 *  - Er loopt een Over The Air (OTA) update service om over Wi-Fi nieuwe code te kunnen uploaden (anders zou dit over de seriele connectie moeten gebeuren, maar die is al in gebruik voor de Arduino).
 *  - Er loopt een Domain Name System (DNS) server om met de ESP te kunnen verbinden zonder het IP adres te kennen.
 *  - Er loopt ook een Multicast DNS (mDNS) service om de ESP te vinden zonder IP adres via het externe AP.
 *  - In het flash geheugen is wordt een file system (FS of SPIFFS) gemaakt, om de bestanden voor de webpagina op te slaan (HTML, CSS, JS, afbeeldingen etc.).
 *  - Er loopt een HTTP web server die de juiste files uit het FS haalt en doorstuurt als een client (web browser) hierom vraagt. De server zal ook geuploade files ontvangen en opslaan, om over Wi-Fi de bestanden en webpagina te kunnen updaten.
 *  - Er loopt een WebSocket (WS) server die commando's over een WS connectie ontvangt. Deze commando's worden verstuurd door de WS client die in de webbrowser loopt.
 *  - Er wordt geluisterd naar UDP packets met commando's. Deze worden verstuurd door de Android app.
 *  - Alles wordt getoond op een OLED schermpje.
 *  - Commando's die over Wi-Fi worden ontvangen, worden gemapt op de juiste commando's voor de Arduino, en verstuurd over de seriele connectie
 *  
 *  ## Commando's
 *  
 *  De commando's die over Wi-Fi verstuurd worden, zijn gewone getallen, beginnend van 0. Deze getallen worden dan omgezet in een commando voor de Arduino: om de code aan de Arduinozijde simpel te houden, zijn deze omgezette commando's dezelfde als de commando's die de IR afstandsbediening uitzendt.
 *  Die omgezette commando's worden dan over de seriele connectie naar de Arduino gestuurd. Dit is het enige wat de ESP doet met de Wi-Fi commando's, ze hebben geen invloed op het schermpje, of het uitzicht van de webpagina etc.
 *  Als de Arduino een commando krijgt (via serieel van de ESP of via IR) stuurt hij dit commando door over serieel naar de ESP. Dan pas zal de ESP dit commando echt interpreteren en zijn schermpje en/of webpagina aanpassen.
 *  Een voorbeeld:
 *  In de app wordt op de knop 'vooruit' gedrukt. Dit commando, bv. 0x01 wordt doorgestuurd naar de ESP over UDP. De ESP mapt dit commando op het overeenkomstige Arduino commando: de 'vooruit' knop op de IR afstandsbediening heeft code 0x61,
 *  dus 0x01 (UDP) wordt gemapt op 0x61 (Arduino) en vervolgens verstuurd naar de Arduino. Daar komt het commando in de seriele inputbuffer terecht, en de Arduino zal het lezen en interpreteren: de motoren zullen in vooruit gezet worden en aangezet met de juiste snelheid. 
 *  Als dit gebeurd is, verzendt de Arduino hetzelfde commando terug naar de ESP. Die zal dan het commando interpreteren en een pijl vooruit tonen op het schermpje. 
 *  
 *  De Arduino doet hetzelfde wanneer een commando over IR ontvangen wordt, en de ESP zal dit commando ook ontvangen en interpreteren, en het schermpje bijwerken.
 */
