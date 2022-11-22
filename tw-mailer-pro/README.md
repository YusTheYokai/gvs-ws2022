# TW-Mailer-Pro
## Cient und Server Architektur

Client und Server sind getrennte Programme, welche in diesem Projekt über eine shared library verfügen.
Beim Start des Servers wird der Port, auf dem dieser laufen soll und der Ordner angegeben, in dem die E-Mails gespeichert werden sollen. Der Client verbindet sich anhand der IP-Adresse und des Ports mit dem Server. Dadurch, dass der Server für jeden Client einen eigenen Thread startet, können mehrere Clients gleichzeitig mit dem Server kommunizieren.

## Verwendete Technologien

Client und Server sind in C++ geschrieben. Das Projekt baut anhand einer Makefile. Für die Kommunikation zwischen Client und Server werden C-Methoden verwendet.  
Für die Authentifizierung wird das LDAP der FH Technikum Wien verwendet.

## Entwicklungsansatz

Die Commands werden sowohl beim Client als auch beim Server in einer Map gespeichert, sodass über den Namen des Befehls der richtige Code ausgeführt wird, ohne unübersichtliche if-else-Verzweigungen zu haben oder case verwenden zu müssen.

Versendete Mails werden im, beim Starten des Servers angegebenen, Ordner gespeichert. Für jede\*n Nutzer\*in existiert hier wiederum ein Ordner mit dessen Namen, in welchem die Mails abgespeichert werden.

Für die Kommunikation zwischen Client und Server haben wir uns dazu entschieden einen 1024 Byte großen Buffer zu verwenden. Sollten längere Nachrichten versendet werden, wird diese nach 1024 Byte abgeschnitten.

Damit wir nicht dauerhaft eine Blocklist up-to-date halten müssen, haben wir uns dazu entschieden alle fehlgeschlagenen Logins in einer Datei zu speichern. Das Format entspricht: [IP-Adresse],[Timestamp]. Somit kann beim Versuch sich einzuloggen geprüft werden, ob der\*die Nutzer\*in, sich innerhalb der letzten Minute bereits versucht hat drei mal einzuloggen. Ist dies der Fall wird der Login abgelehnt, bevor ein Authentifizierungsversuch am LDAP erfolgt.

## Abweichungen / Zusätze der Angabe

Der Nutzername muss mindestens 3 Zeichen lang sein. Für uns macht es keinen Sinn, dass ein\*e Nutzer\*in mit einem Namen wie "a" oder "ab" existiert.

Der Betreff einer Mail kann beliebig lang sein, wir möchten die Nutzer*innen hier nicht beschränken.

## Verbesserungsmöglichkeiten

Nachrichten die länger als der Buffer sind könnten in mehrere Pakete aufgeteilt werden, um tatsächlich unendlich lange Nachrichten zu ermöglichen.
