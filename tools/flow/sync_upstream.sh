#!/bin/bash
# Flow DaVinci: integrare controlata a schimbarilor din OpenGate (principal) si Unbound (secundar).
# Utilizare:
#   tools/flow/sync_upstream.sh status            # ce este nou in upstream (nu modifica nimic)
#   tools/flow/sync_upstream.sh merge-opengate    # ramura sync/opengate-<data>, merge fara commit, re-aplica identitatea si traducerea
#   tools/flow/sync_upstream.sh review-unbound    # commit-uri Unbound care nu sunt in istoricul nostru (cherry-pick manual, -x)
# Nu descarca sau executa cod upstream pe calculatoarele utilizatorilor; lucreaza doar in depozitul de dezvoltare.
set -eu
root="$(cd "$(dirname "$0")/../.." && pwd)"; cd "$root"
git remote get-url upstream >/dev/null 2>&1 || git remote add upstream https://github.com/jordantetewsky/OpenGate.git
git remote get-url unbound  >/dev/null 2>&1 || git remote add unbound  https://github.com/wolfgangw/Unbound.git
git fetch -q upstream; git fetch -q unbound
case "${1:-status}" in
 status)
  echo "== OpenGate: commit-uri noi fata de HEAD"; git log --oneline HEAD..upstream/main | head -40
  echo "== Unbound: commit-uri care nu sunt in HEAD"; git log --oneline HEAD..unbound/master | head -40
  echo "== Fisiere modificate de noi si de upstream (posibile conflicte)"
  base=$(git merge-base HEAD upstream/main)
  comm -12 <(git diff --name-only "$base" HEAD | sort) <(git diff --name-only "$base" upstream/main | sort) ;;
 merge-opengate)
  [[ -z "$(git status --porcelain)" ]] || { echo 'Arborele de lucru trebuie sa fie curat.'; exit 1; }
  branch="sync/opengate-$(date +%Y%m%d)"; git checkout -b "$branch"
  if ! git merge --no-ff --no-commit upstream/main; then
   echo 'CONFLICTE: la fiecare fisier ia varianta upstream pentru liniile in conflict (git checkout --theirs / editare), apoi ruleaza pasii de mai jos.'
  fi
  python3 tools/flow/apply_identity.py --apply | head -1
  python3 tools/flow/wrap_ui_text.py --apply >/dev/null
  python3 tools/flow/check_catalog.py || echo 'ATENTIE: texte noi fara traducere — adauga-le in src/core/flowlocale_ro.h.'
  echo "Urmatorii pasi: rezolva conflictele ramase, ruleaza testele (doc/flow/SYNC.md), apoi: git commit." ;;
 review-unbound)
  git log --oneline --no-merges HEAD..unbound/master | head -60
  echo 'Alege manual commit-urile relevante si compatibile: git cherry-pick -x <sha>; apoi re-aplica identitatea (apply_identity.py/wrap_ui_text.py).' ;;
 *) echo 'Comanda necunoscuta'; exit 2 ;;
esac
