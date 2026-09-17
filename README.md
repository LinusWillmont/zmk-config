# zmk-config — Lily58 (nice!nano v2 + nice!view gem)

ZMK firmware config for a split Lily58. Board: `nice_nano_v2`, shields:
`lily58_left` / `lily58_right` + `nice_view_adapter nice_view_gem`. Keymap:
[`config/lily58.keymap`](config/lily58.keymap), Kconfig: [`config/lily58.conf`](config/lily58.conf).

## Layers

| # | name          | how to reach                | what                                                      |
|---|---------------|-----------------------------|-----------------------------------------------------------|
| 0 | `default`     | —                           | US QWERTY                                                 |
| 1 | `lower`       | hold LOWER (left thumb)     | F-keys, symbols, BT select (`BT1`,`BT2` on `3`,`4`), BT clear (`5`), ext power (`Z X C`) |
| 2 | `raise`       | hold RAISE (right thumb)    | numpad, arrows, `å ä ö` (Windows), `+ - =`, `\`, CAPS      |
| 3 | `linux_os`    | **LOWER + `1`** (toggle)    | marks host as Linux; fully transparent otherwise           |
| 4 | `raise_linux` | auto: RAISE while 3 is on   | same `å ä ö` positions, Linux-compatible keycodes          |

`å ä ö` positions on RAISE (right half, outer column): `-`→`å`, `'`→`ä`, `;`→`ö`. Shift gives capitals.

### Why two OS modes

* **Windows**: `å ä ö` are macros sending `Alt` + numpad `134`/`132`/`148` (Alt-codes). Works on any US layout.
* **Linux**: Alt-codes don't exist. `raise_linux` sends `AltGr+W` / `AltGr+Q` / `AltGr+P` instead, which are
  `å ä ö` in xkb's `us(altgr-intl)` variant. Shift passes through → `Å Ä Ö`.

Toggle with LOWER + `1`. The toggle is per-keyboard state, not per BT profile — flip it when you move between machines.
Nothing in `linux_os` is bound except that override, so leaving it on by mistake only breaks `å ä ö`.

### Linux host setup (one time)

Set the `us` layout variant to `altgr-intl`. Fedora / COSMIC: `~/.config/cosmic/com.system76.CosmicComp/v1/xkb_config`:

```
layout: "us,se",
variant: "altgr-intl,",
```

GNOME: Settings → Keyboard → add "English (intl., with AltGr dead keys)". `localectl set-x11-keymap us pc105 altgr-intl` for the console/login screen.
Verify: `AltGr+W` in any text field should type `å`.

## Building

CI builds on every push (`.github/workflows/build.yml` → ZMK's `build-user-config.yml`). No local toolchain needed.

1. Edit `config/lily58.keymap`, commit, push.
2. GitHub → Actions → latest run → Artifacts → `firmware` (zip). Or:
   ```sh
   gh run watch -R LinusWillmont/zmk-config            # wait for green
   gh run download -R LinusWillmont/zmk-config -n firmware -D firmware
   ls firmware/   # lily58_left-…-zmk.uf2  lily58_right-…-zmk.uf2  settings_reset-…-zmk.uf2
   ```
3. Flash (below).

Keymap edits only need the **left (central) half** flashed. Flash both when changing `west.yml`, ZMK version,
shields, or `.conf`.

## Flashing

nice!nano runs a UF2 bootloader that shows up as a USB drive called `NICENANO`.

1. Plug the half into USB.
2. Double-tap the reset button quickly (or short `RST`→`GND` twice). The `NICENANO` drive appears.
3. Copy the matching `.uf2` onto it. The board reboots itself and the drive disappears; that's success.

```sh
# Linux (Fedora automounts under /run/media/$USER/NICENANO)
cp firmware/lily58_left*.uf2 /run/media/$USER/NICENANO/
# if it didn't automount:
sudo mount /dev/disk/by-label/NICENANO /mnt && sudo cp firmware/lily58_left*.uf2 /mnt && sudo umount /mnt
```

```powershell
# Windows: NICENANO shows up as a drive letter
copy firmware\lily58_left*.uf2 E:\
```

Flash the **left** half with `lily58_left…uf2`, the **right** with `lily58_right…uf2`. Wrong side = halves won't pair.

### Halves not pairing / weird BT state

Flash `settings_reset-…uf2` to **both** halves (wipes bonds), then flash the normal left/right firmware again,
then re-pair with the host (forget the device on the host too, LOWER+`5` = `BT_CLR_ALL` on the keyboard).

## Bluetooth

* LOWER + `3` / `4`: profile 0 / 1.
* LOWER + `5`: clear all profiles.
* USB takes priority over BT when plugged in.
