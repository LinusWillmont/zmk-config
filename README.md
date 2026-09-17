# zmk-config — Lily58 (nice!nano v2 + nice!view gem)

ZMK firmware config for a split Lily58. Board: `nice_nano//zmk` (nice!nano v2, ZMK board variant since Zephyr 4.1), shields:
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

Toggle with LOWER + `1`. The toggle is per-keyboard state, not per BT profile, and **resets when the keyboard
reboots/power-cycles** — flip it when you move between machines or after a restart. While Linux mode is on the
**right** nice!view shows Tux instead of the gem; the left layer label keeps showing the real layer.
Nothing in `linux_os` is bound except that override, so leaving it on by mistake only breaks `å ä ö`.

### Linux host setup (one time)

Set the `us` layout variant to `altgr-intl`. Fedora / COSMIC: `~/.config/cosmic/com.system76.CosmicComp/v1/xkb_config`:

```
layout: "us,se",
variant: "altgr-intl,",
```

GNOME: Settings → Keyboard → add "English (intl., with AltGr dead keys)". `localectl set-x11-keymap us pc105 altgr-intl` for the console/login screen.
Verify: `AltGr+W` in any text field should type `å`.

## Display (nice!view gem)

The [nice-view-gem](https://github.com/M165437/nice-view-gem) shield is **vendored** in `boards/shields/nice_view_gem`
(no west dependency) so it can be patched. Local additions, set in `config/lily58.conf`:

| option                              | what                                                     |
|-------------------------------------|----------------------------------------------------------|
| `CONFIG_NICE_VIEW_GEM_PROFILE_COUNT`| number of BT profile dots drawn (2)                      |
| `CONFIG_NICE_VIEW_GEM_OS_LAYER`     | layer index = Linux mode; hidden from label, drives Tux (3) |
| `CONFIG_NICE_VIEW_GEM_OS_SYNC`      | forward that state to the peripheral (default y on split) |

How the right half knows: ZMK has no layer-state forwarding to peripherals, but it does forward the host's HID LED
byte (Caps/Num/Scroll...) when `CONFIG_ZMK_SPLIT_PERIPHERAL_HID_INDICATORS=y`. `widgets/os_sync.c` on the central
re-sends that byte with **bit 7** set while layer 3 is active (50 ms after any layer/LED/endpoint change, plus every
10 s so a rebooted right half catches up). `widgets/screen_peripheral.c` swaps the gem for `assets/tux.c` on that bit.

Tux art: `assets/tux.c`, 64×64 1-bit, 8 bytes/row, MSB = leftmost pixel, `1` = ink. Made from the kernel's
`drivers/video/logo/logo_linux_mono.pbm` scaled to 64 px, threshold 80, then **rotated 90° clockwise** because art is
stored in frame orientation (frame right edge = physical top; the crystal frames are stored the same way). Any 64×64
`#`/`.` text grid converts the same way.

## Building

CI builds on every push (`.github/workflows/build.yml` → ZMK's `build-user-config.yml`). No local toolchain needed.

1. Edit `config/lily58.keymap`, commit, push to `master` (CI only builds `master`; or Actions → Run workflow).
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
