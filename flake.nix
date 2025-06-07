{
  inputs = {
    nixpkgs.url = github:NixOS/nixpkgs/nixos-unstable;
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {self, nixpkgs, flake-utils, ...}:
  flake-utils.lib.eachDefaultSystem (
    system:
    let
      pkgs = nixpkgs.legacyPackages.${system};
    in rec {
      packages.default = pkgs.gcc12Stdenv.mkDerivation {
        name="graffiks";
        src = ./.;
        /* NIX_DEBUG=7; */
        buildInputs = with pkgs; [ cmake libGL glew xorg.libX11 ];
      };
    }
  );
}
