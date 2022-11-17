# TW-Mailer-Basic
## Cient und Server Architektur

Client und Server sind getrennte Programme, welche in diesem Projekt über eine shared library verfügen.
Beim Start des Servers wird der Port, auf dem dieser laufen soll und der Ordner angegeben, in dem die E-Mails gespeichert werden sollen. Der Client verbindet sich anhand der IP-Adresse und des Ports mit dem Server.

## Verwendete Technologien

Client und Server sind in C++ geschrieben. Das Projekt baut anhand einer Makefile. Für die Kommunikation zwischen Client und Server werden C-Methoden verwendet.

## Entwicklungsansatz

Die Commands werden sowohl beim Client als auch beim Server in einer Map gespeichert, sodass über den Namen des Befehls der richtige Code ausgeführt wird, ohne unübersichtliche if-else-Verzweigungen zu haben oder case verwenden zu müssen.

Versendete Mails werden im, beim Starten des Servers angegebenen, Ordner gespeichert. Für jede\*n Nutzer\*in existiert hier wiederum ein Ordner mit dessen Namen, in welchem die Mails abgespeichert werden. Der Dateiname einer Mail ist der Betreff selbst, somit muss für den _LIST_-Befehl keine Datei geöffnet werden.

Für die Kommunikation zwischen Client und Server haben wir uns dazu entschieden einen 1024 Byte großen Buffer zu verwenden. Sollten längere Nachrichten versendet werden, wird diese nach 1024 Byte abgeschnitten.

## Abweichungen / Zusätze der Angabe

Der Nutzername muss mindestens 3 Zeichen lang sein. Für uns macht es keinen Sinn, dass ein\*e Nutzer\*in mit einem Namen wie "a" oder "ab" existiert.

Der Betreff einer Mail kann beliebig lang sein, wir möchten die Nutzer*innen hier nicht beschränken.

## Verbesserungsmöglichkeiten

Der Server des TW-Mailer-Basics ist, wie der Angabe entsprechend, single-threaded. Jedoch ist es möglich, dass sich auch ein zweiter Client verbindet. Da die erste Kommunikation zwischen Client und Server erst beim Senden des ersten Commands zustandekommt wartet der Client hier, bis der bereits verbundene Client die Verbindung getrennt hat. Nutzer*innen des Clients erhalten hier kein Feedback, wieso sie so lange auf eine Antwort warten müssen. Hier wäre entweder eine entsprechende Nachricht als Information oder der Ausbau des Servers auf multi-threaded angebracht.

Nachrichten die länger als der Buffer sind könnten in mehrere Pakete aufgeteilt werden, um tatsächlich unendlich lange Nachrichten zu ermöglichen.
