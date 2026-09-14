##1 => Panning

* il faut déjà faire une réglage de panning -100 > +100 sur les bus stereo
à 0 = 100% sur les deux canaux
à - 100 = 100% sur le canaux gauche
à + 100 = 100 % sur le canaux droit

##2 => + canal Sample & + canal Master

- 1 canal stéréo Sample supplémentaire sur 7 & 8
- 1 canal stéréo Master supplémentaire sur 7 & 8

##3 => 1 note MIDI = plusieurs samples sur plusieurs canaux
comme ça on peut trigger les 4 canaux en même temps

##4 => LFO Quadriphonic modulation

* on peut mettre en place un LFO quadri de volume automatique et un LFO quadri de filtering
avec un CC pour le amount et un CC pour la vitesse
les phases sont pré-réglées par canaux
il distribue leurs signaux au deux bus

les réglages de filtres + volume existant servent de base

(à voir si on ajoute des MIDI cc pour controler ça dans le MIDI Controller ???) => je pense que non

##5 => mod distribution random
le LFO change de valeur quand ça trig ?
ou alors on utilise un autre modulator dédié à ça

-----


