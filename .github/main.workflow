workflow "New workflow" {
  on = "push"
  resolves = ["Check CHANGELOG", "Static Analysis"]
}

action "Check CHANGELOG" {
  uses = "./.github/actions/bin/diff-check"
  args = ["CHANGELOG.md"]
}

action "Static Analysis" {
  uses = "./.github/actions/static-analysis"
}