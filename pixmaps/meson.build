install_data(
  files(
    'hicolor/16x16/actions/package-available-locked.png',
    'hicolor/16x16/actions/package-available.png',
    'hicolor/16x16/actions/package-broken.png',
    'hicolor/16x16/actions/package-downgrade.png',
    'hicolor/16x16/actions/package-install.png',
    'hicolor/16x16/actions/package-installed-locked.png',
    'hicolor/16x16/actions/package-installed-outdated.png',
    'hicolor/16x16/actions/package-installed-updated.png',
    'hicolor/16x16/actions/package-new.png',
    'hicolor/16x16/actions/package-purge.png',
    'hicolor/16x16/actions/package-reinstall.png',
    'hicolor/16x16/actions/package-remove.png',
    'hicolor/16x16/actions/package-upgrade.png',
    'hicolor/16x16/actions/system-upgrade.png',
  ),
  install_dir: pkg_datadir / 'icons' / 'hicolor' / '16x16' / 'actions',
)

# Distro specific package-supported icon

lsb_release = find_program('lsb_release', required: true)

result = run_command('lsb_release', '-is', check: true)
distro = result.stdout().strip().to_lower()

fs = import('fs')

image = 'hicolor/16x16/actions/package-supported.png'
candidate = image + '.' + distro

if fs.exists(candidate)
  image = candidate
endif

install_data(
  files(image),
  install_dir: pkg_datadir / 'icons' / 'hicolor' / '16x16' / 'actions',
  rename: ['package-supported.png']
)

#

install_data(
  files('hicolor/24x24/actions/system-upgrade.png'),
  install_dir: pkg_datadir / 'icons' / 'hicolor' / '24x24' / 'actions',
)

install_data(
  files('hicolor/256x256/apps/synaptic.png'),
  install_dir: datadir / 'icons' / 'hicolor' / '256x256' / 'apps',
)

install_data(
  files('hicolor/scalable/apps/synaptic.svg'),
  install_dir: datadir / 'icons' / 'hicolor' / 'scalable' / 'apps',
)
