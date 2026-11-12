{
  "cps_version": "0.13",
  "name": "ScopeReq",
  "cps_path": "@prefix@/cps",
  "requires": {
    "ScopeDep": {
      "components": [ "target" ]
    }
  },
  "components": {
    "target": {
      "type": "interface",
      "requires": [ "ScopeDep:target" ]
    }
  }
}
