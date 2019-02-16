"Copyright [2019] Gavan Adrian-George, 334CA"

Nume, prenume: Gavan, Adrian-George

Grupa, seria: 334CA

Tema 3 EGC - New Vegas

Taskuri:
========

- Generarea procedurala a geometriei cladirilor - a fost implementata
- Generarea procedurala a retelei de drumuri - a fost implementata
- Texturarea cladirilor - nu a fost implementata (cladirile au culoarea albastra)
- Texturarea strazilor - a fost implementata
- Iluminare scena - a fost implementata (spoturi + lumina directionala)

!Observatii!:
===========

- Se genereaza multe obiecte si tema poate merge greu. Pentru a se misca mai rapid 
se pot genera mai putine cladiri cu 2-3 nivele => la linia 962 in loc de %26 sa se
puna un numar mult mai mare (%26000 de exemplu, pentru ca astfel sansa ca o cladire 
sa aiba mai mult de un etaj este extrem de mica).

- Din folderul "Resources/Models" s-a sters folderul "Characters" pentru ca altfel arhiva 
depasea 20MB, iar acest folder nu era utilizat la nimic. Tema poate fi testata si fara 
acest folder, folderul neinfluentand cu absolut nimic.

Interactiune:
=============

- La pornirea programului va aparea orasul generat.
- Utilizatorul se poate misca in scena tinand Click Dreapta apasat + tastele W,A,S,D,Q,E.
- Daca se apasa tasta R se va regenera orasul (se poate tine apasat o secunda pentru 
siguranta).
- Cu tastele U,J,H,K,Y,I se modifica pozitia sursei de lumina directionala in scena (ca 
un fel de soare).

Generare strazi:
================

- Mai intai vor fi generate strazile.
- Orasul se incadreaza stanga - dreapta intre -25, +25 si sus jos tot intre -25, +25.
- Orasul va fi mereu inconjurat de 4 strazi cu latimea de 1 unitate (un fel de 
contur al orasului, ca o centura).
- Orasul va avea de asemnea 2 "bulevarde" centrale, cu centrul in X = 0 si Z = 0, 
unul vertical si unul orizontal, cu latimea intre 0.5 si 1 unitati.
- Se genereaza mai intai strazile pe "verticala" (lungi pe axa OZ) de la stanga la dreapta.
- Inafara de strazile ce inconjoara orasul (cele 2 verticale) si bulevardul principal 
vertical, se vor genera ale maxim 22 de strazi (11 la stanga de axa OZ si 11 la dreapta).
- Fiecare strada va avea centrul intr-o anumita pozitie determinata (pentru a a sigura o 
distanta de minim o unitate intre 2 strazi consecutive).
- Cu toate ca pot fi generate 22 de strazi, nu toate vor fi generate. Daca o strada nu 
este generata, urmatoarea strada va fi generata tot in pozitia ei specifica.
- Fiecare dintre aceste 22 de strazi are o sansa de 70% sa fie creata sau nu (se foloseste 
functia rand()).
- Aceste maxim 22 de strazi au latime variabila, intre 0.1 si 1 unitati.
- Strazile de pe "orizontala" (lungi pe axa OX) se formeaza tot dupa algoritmul de mai sus.
- Toate strazile verticale se pun de la stanga la dreapta in vectorul cu strazi pe 
verticala, iar cele pe orizontala se pun din spate in fata in vectorul cu strazi pe 
orizontala.
- Strazile sunt cuburi scalate dupa anumite dimensiuni.
- Spatiile dintre strazi vor fi dreptunghiuri, iar in aceste spatii se vor plasa cladirile.
- Aceste spatii vor avea o suprata minima de un patrat cu latura de o unitate.

Generare cladiri:
=================

- Cladirile sunt formate prin suprapunerea a diferite figuri geometrice, de diferite 
dimensiuni si rotite diferit.
- Cladirile pot avea maxim 3 nivele (3 figuri suprapuse).
- Se incepe prin parcurgerea fiecarui spatiu dintre strazi. Se vor parcurge spatiile de la 
stanga la dreapta si din spate in fata.
- Se calculeaza lungimea si latimea spatiului curent.
- Exista 2 cazuri: sa fie doar o cladire mare pe un spatiu, sau mai multe cladiri dar mai 
mici (mai mici ca latime cel putin). Poate fi doar o cladire mare pe un spatiu doar daca 
spatiul are laturile mai mici sau egale cu 1.5 unitati. Daca una din laturi este mai mare 
=> avem mai multe cladiri mai mici.
- Se verifica laturile spatiului.
- Daca ambele laturi sunt mai mici sau egale cu 1.5 unitati => poate fi o cladire mare sau
mai multe mai mici. Se alege random (sanse 50%) daca pe un astfel de spatiu va fi doar 
o cladire mare sau mai multe mai mici.
- Cazul cand trebuie formata doar o cladire mare:
    - O cladire mare poate avea doar 2 nivele.
    - Se calculeaza valorile pentru ambele nivele, dar daca cladirea nu va avea nivel 2 
    acesta doar nu va fi adaugat in lista cu elemente ale cladirilor.
    - Se alege rotatia nivelelor.
    - Se alege tipul nivelului 1 (forma geometrica).
    - Se alege random daca exista sau nu si nivel 2 (50%). Daca exista, va avea aceeasi 
    forma ca si primul nivel.
    - Scalarea maxima a unei cladiri mari va fi minumul dimensiunii laturilor spatiului.
    - Se calculeaza random valorile de scalare ale celor 2 nivele.
    - Se calculeaza pozitia celor 2 nivele (in centrul spatiului).
    - Se adauga obiectele in vectorul cu elementele cladirilor. Daca cladirea are doar 
    un nivel => obiectul de nivel 2 nu va fi adaugat in lista.
- Cazul cand trebuie formate mai multe cladiri mai mici
    - O cladire "mica" poate fi scalata pe OX si OZ la maxim 0.4 unitati si poate avea 
    3 nivele.
    - Se parcurge spatiul dintre strazi din 0.5 in 0.5 (plecand de la 0.25 de langa strada 
    verticala din stanga si 0.25 fata de strava orizontala din spate) tocmai pentru a avea 
    loc de scalari (maxim 0.4 + o dinstanta de 0.1 intre cladiri => 0.5).
    - In fiecare loc se genereaza o noua cladire.
    - Se alege rotatia nivelelor.
    - Se alege tipul figurii geometrice al primului nivel.
    - Se alege random daca exista nivel 2 sau ce tip de figura geometrica va avea.
    - Se alege random daca exista nivel 3 sau ce tip de figura geometrica va avea. Daca nu 
    exista nivel 2 => automat nu va exista nici nivel 3.
    - Se aleg random factorii de scalare pentru cele 3 nivele.
    - Se calculeaza pozitia obiectelor si se adauga in vectorul cu elemente ale claridilor.
    - Daca nivelul 2 sau 3 nu exista => nu se adauga elementele corespunzatoare in vector.

Reguli cladiri:
===============

a) Cladire mare
- 4 pozitii de rotatie (se alege random si ambele nivele au aceeasi rotatie)
- Se alege random tipul primului nivel.
- Se alege random (50%) daca exista sau nu al doilea nivel. Daca exista are acelasi tip 
ca si primul nivel.
- Nivelul 1 va fi scalat pe x si z intre 0.2 si "max_scale" (unde max_scale = minimul 
laturilor). Pe y va fi scalat intre 0.5 si 4 daca nu va fi nivel 2 si intre 0.5 si 2 daca 
va fi nivel 2.
- Daca si X si Z au valori mai mici decat 0.5 => se va alege random care din ele va fi 
scalat intre 0.8 si max_scale.
- Nivelul 2 va fi scalat pe x si z aceeasi valoare cu nivelul 1 sau cu o valaoare mai mica 
cu maxim 0.1 ca si nivelul 1. Pe y va fi scalat intre 0.5 si 2.
- Obiectele vor fi plasate in centrul suprafetei curente.

b) Cladiri mici
- 8 pozitii de rotatie (se alege random si toate nivele au aceeasi rotatie)
- Se alege random tipul primului nivel.
- Tipul nivelului 2 se alege random. Exista sanse sa nu fie, iar daca exista are sanse mai 
mari sa fie de acelasi tip ca si nivelul 1.
- Tipul nivelului 3 se alege random. Daca nivelul 2 nu exista => automat si tipul nivelului 
3 este "null". Daca exista nivelul 2, nivelul 3 are sanse sa nu exise. Daca exista, 
are sanse mai mari sa fie de acelasi fel ca si nivelul 2. Nivelul 3 poate avea 3 forme 
diferite fata de celelalte nivele (sfera, con sau piramida).
- Nivelul 1 va fi scalat pe x si z intre 0.2 si 0.4.
- Daca nu exista alt nivel, va fi scalat pe y intre 0.2 si 2.
- Daca exista si alte nivele, va fi scalat pe y intre 0.2 si 1.
- Nivelele superioare vor fi scalate pe x si z astfel:
    - Daca au acelasi tip ca nivelul anterior => valori intre valoarea de nivel anterior
    si valoarea de nivel anterior - 0.05.
    - Daca au tip diferit ca nivelul anterior => au valoare nivelului anterior - 0.05.
- Nivelul 2 poate fi scalat pe y intre 0.2 si 1.
- Nivelul 3 poate fi scalat pe y intre 0.1 si 1. Exceptie fac: conul, piramida si sfera.
Acestea pot fi scalate doar intre 0.1 si 0.3.
- Exceptie: Daca nivelul 1 este de tipul "prisma_triunghiulara" => toate nivelele vor 
fi de tipul prisma triunghiulara.

Generare elemente decorative:
=============================

- Scena prezinta 3 elemente decorative: banci, oameni (rol de politist) si elicoptere.
- Generare banci:
    - Exista cate o banca in fiecare spatiu cu cladiri.
    - Banca va fi pusa la jumatatea unei laturi a spatiului (si orientata cu fata spre 
    strada).
    - Se alege random pe ce latura va fi pozitionata banca.

- Generare oameni (politisti):
    - Vor exista un numar de oameni = numarul de intersectii / 4.
    - Oamenii vor fi amplasati in intersectii diferite.
    - Intersectia si orientarea omului va fi aleasa random.

- Generare elicoptere:
    - Vor fi generate 10 elicoptere.
    - Rotatia lor va fi random.
    - Plasarea lor va fi random deasupra orasului (fara a intersecta cladiri)

Generare spoturi:
=================

- Cand se creeaza elementele unei cladiri sunt create si spoturile.
- O cladire poate avea maxim 4 spoturi, cate unul in fiecare parte.
- Este o sansa de 50% ca un spot sa existe (poate sa nu fie niciunul).
- Cum se stie in ce parte va fi un anumit spot, se va sti si intre ce valori trebuie 
setata directia luminii.
- Directia luminii va fi -1 pe axa OY si intre 0.4 si 0.8 (sau -0.4 si -0.8) pe axa 
corespunzatoare.
- Distanta luminii fata de latura corespunzatoare a cladirii va fi intre 0.2 si 0.7.
- Inaltimea pozitiei unui spot va fi intre 0.2 si inaltimea cladirii + 0.5.
- Se adauga spoturile in vectorul de spoturi al fiecarui element al cladirii.
