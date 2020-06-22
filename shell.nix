{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    libftdi
    libusb1
    ncurses5
    gcc-arm-embedded
    gcc
    gnumake
    pkgconfig
    git
  ];

  HOSTCC = "gcc";
}
