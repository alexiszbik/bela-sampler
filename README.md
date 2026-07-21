# Untitled — Bela Gem

Projet C++ Bela Gem, développé localement sur Mac et déployé sur la carte via SSH.

## Prérequis

- Bela Gem connectée en USB-C au Mac
- Connexion SSH fonctionnelle : `ssh root@bela.local`
- `rsync` installé (inclus sur macOS)

## Déploiement rapide

```bash
# Compiler et lancer sur la Gem (440 Hz sur les sorties audio)
./scripts/deploy.sh

# Compiler sans lancer
./scripts/deploy.sh -n

# Rebuild propre si les changements ne semblent pas pris en compte
./scripts/deploy.sh --clean

# Arrêter le programme en cours
./scripts/stop.sh
```

Le projet est copié sur la carte dans `/root/Bela/projects/RenderTest/`, puis compilé avec le Makefile Bela de la carte.

## Structure

| Fichier | Rôle |
|---------|------|
| `render.cpp` | Code audio (setup / render / cleanup) |
| `settings.json` | Paramètres du projet (block size, gains, etc.) |
| `scripts/deploy.sh` | Sync SSH + build + run |
| `scripts/stop.sh` | Arrête le programme Bela |
| `scripts/run.sh` | Relance un build déjà déployé |

## Connexion

Par défaut, les scripts utilisent `root@bela.local`. Si la résolution mDNS ne fonctionne pas, essayez :

```bash
BBB_HOSTNAME=192.168.7.2 ./scripts/deploy.sh
```

Pour une config permanente, créez `~/.belarc` :

```bash
[ -z "$BBB_HOSTNAME" ] && BBB_HOSTNAME="bela.local"
```

## IDE embarqué

Vous pouvez aussi éditer et lancer le projet depuis le navigateur : [http://bela.local](http://bela.local)

## Documentation

- [Bela Gem — Get Started](https://learn.bela.io/get-started-guide/bela-gem-get-started/)
- [C++ sur Bela](https://learn.bela.io/using-bela/languages/c-plus-plus/)
- [Scripts Bela](https://learn.bela.io/using-bela/technical-explainers/scripts/)
