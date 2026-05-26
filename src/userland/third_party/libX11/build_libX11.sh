#!/bin/bash
# build_libX11.sh — cross-compile libX11 1.3.5 for BoredOS
# Uses x86_64-elf-gcc + mlibc, Unix-socket transport only, no XCB/locale/threads.
set -e

# Resolve repo root from env, git, or script location.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BOREDOS_ROOT="${BOREDOS_ROOT:-}"
if [ -z "$BOREDOS_ROOT" ]; then
  if command -v git >/dev/null 2>&1; then
    BOREDOS_ROOT="$(git -C "$SCRIPT_DIR" rev-parse --show-toplevel 2>/dev/null || true)"
  fi
  if [ -z "$BOREDOS_ROOT" ]; then
    BOREDOS_ROOT="$(cd "$SCRIPT_DIR/../../../../" && pwd)"
  fi
fi
if [ ! -f "$BOREDOS_ROOT/Makefile" ]; then
  echo "[ERR] Could not resolve BoredOS root (missing Makefile)." >&2
  exit 1
fi
MLIBC_HEADERS="${BOREDOS_ROOT}/src/userland/mlibc/build/mlibc-headers/usr/include"
SDK_INCLUDE="${BOREDOS_ROOT}/src/userland/sdk/include"
XORGPROTO="${BOREDOS_ROOT}/src/userland/third_party/xorgproto/include"
XTRANS="${BOREDOS_ROOT}/src/userland/third_party/xtrans"
LIBX11_INC="${BOREDOS_ROOT}/src/userland/third_party/libX11/include"

CC="x86_64-elf-gcc"
AR="x86_64-elf-ar"

CFLAGS=(
  "-ffreestanding"
  "-O2"
  "-fno-stack-protector"
  "-fno-stack-check"
  "-fno-lto"
  "-fno-pie"
  "-m64"
  "-march=x86-64"
  "-mno-red-zone"
  "-U__INT_FAST8_TYPE__"  "-D__INT_FAST8_TYPE__=signed char"
  "-U__UINT_FAST8_TYPE__" "-D__UINT_FAST8_TYPE__=unsigned char"
  "-U__INT_FAST16_TYPE__" "-D__INT_FAST16_TYPE__=long int"
  "-U__UINT_FAST16_TYPE__" "-D__UINT_FAST16_TYPE__=long unsigned int"
  "-U__INT_FAST32_TYPE__" "-D__INT_FAST32_TYPE__=long int"
  "-U__UINT_FAST32_TYPE__" "-D__UINT_FAST32_TYPE__=long unsigned int"
  "-D_GNU_SOURCE"
  "-D_DEFAULT_SOURCE"
  "-DHAVE_CONFIG_H"
  "-DUNIXCONN"
  "-DX11_t"
  "-DTRANS_CLIENT"
  # Disable optional features
  "-DNARROWPROTO"
  "-DNO_XLOCALEDIR"
  "-DX_LOCALE"
  # Tell xtrans we want unix only
  "-DUSE_POLL"
  # Suppress known-harmless warnings from old code
  "-Wno-deprecated-declarations"
  "-Wno-implicit-function-declaration"
  "-Wno-implicit-int"
  "-Wno-int-conversion"
  "-Wno-pointer-sign"
  "-Wno-missing-prototypes"
  "-Wno-strict-prototypes"
  "-Wno-missing-declarations"
  "-Wno-unused-parameter"
  "-Wno-sign-compare"
  "-Wno-unused-variable"
  "-Wno-address"
  "-Wno-shadow"
  # Include paths (order matters)
  "-isystem" "${MLIBC_HEADERS}"
  "-I."
  "-Isrc"
  "-Isrc/xlibi18n"
  "-Isrc/xcms"
  "-I${LIBX11_INC}"
  "-I${LIBX11_INC}/X11"
  "-I${XORGPROTO}"
  "-I${XTRANS}"
  "-I${SDK_INCLUDE}"
)

# Core Xlib files (no xcb_io, no xcb_disp, no i18n modules, no xcms, no xkb)
SRCS=(
  # Connection / transport
  src/x11_trans.c
  src/ConnDis.c
  src/OpenDis.c
  src/ClDisplay.c
  src/XlibAsync.c
  src/XlibInt.c
  src/locking.c
  # Event handling
  src/NextEvent.c
  src/PeekEvent.c
  src/PeekIfEv.c
  src/IfEvent.c
  src/MaskEvent.c
  src/ChkIfEv.c
  src/ChkMaskEv.c
  src/ChkTypEv.c
  src/ChkTypWEv.c
  src/ChkWinEv.c
  src/WinEvent.c
  src/FilterEv.c
  src/PutBEvent.c
  src/EvToWire.c
  src/SendEvent.c
  src/evtomask.c
  src/GetMoEv.c
  # Window management
  src/CrWindow.c
  src/ChWindow.c
  src/MoveWin.c
  src/DestWind.c
  src/MapWindow.c
  src/MapRaised.c
  src/MapSubs.c
  src/UnmapWin.c
  src/UnmapSubs.c
  src/ConfWind.c
  src/ReconfWin.c
  src/RaiseWin.c
  src/LowerWin.c
  src/CirWin.c
  src/CirWinDn.c
  src/CirWinUp.c
  src/ChWAttrs.c
  src/GetWAttrs.c
  src/GetGeom.c
  src/QuTree.c
  src/TrCoords.c
  src/RepWindow.c
  # Properties & atoms
  src/ChProp.c
  src/DelProp.c
  src/GetProp.c
  src/LiProps.c
  src/RotProp.c
  src/SetSOwner.c
  src/GetSOwner.c
  src/ConvSel.c
  src/IntAtom.c
  src/GetAtomNm.c
  src/PropAlloc.c
  # Graphics contexts
  src/CrGC.c
  src/ChGC.c
  src/CopyGC.c
  src/FreeGC.c
  src/GetGCVals.c
  src/GCMisc.c
  src/SetFont.c
  src/SetFore.c
  src/SetFunc.c
  src/SetState.c
  src/SetStip.c
  src/SetTile.c
  src/SetClMask.c
  src/SetClOrig.c
  src/SetDashes.c
  src/SetLStyle.c
  src/SetPMask.c
  src/SetTSOrig.c
  src/SetBack.c
  src/SetCRects.c
  # Drawing
  src/DrLine.c
  src/DrLines.c
  src/DrPoint.c
  src/DrPoints.c
  src/DrRect.c
  src/DrRects.c
  src/DrArc.c
  src/DrArcs.c
  src/DrSegs.c
  src/FillArc.c
  src/FillArcs.c
  src/FillPoly.c
  src/FillRct.c
  src/FillRcts.c
  src/Clear.c
  src/ClearArea.c
  src/CopyArea.c
  src/CopyPlane.c
  src/ImText.c
  src/ImText16.c
  src/PolyTxt.c
  src/PolyTxt16.c
  src/Text.c
  src/Text16.c
  # Image
  src/PutImage.c
  src/GetImage.c
  src/ImUtil.c
  # Fonts (core X fonts — NOT Xft)
  src/Font.c
  src/LoadFont.c
  src/UnldFont.c
  src/FontInfo.c
  src/FontNames.c
  src/GetFProp.c
  src/GetFPath.c
  src/SetFPath.c
  src/FSWrap.c
  src/TextExt.c
  src/TextExt16.c
  src/QuTextE16.c
  src/QuTextExt.c
  src/StBytes.c
  # Colormap
  src/CrCmap.c
  src/CopyCmap.c
  src/FreeCmap.c
  src/LiICmaps.c
  src/InsCmap.c
  src/UninsCmap.c
  src/GetStCmap.c
  src/SetStCmap.c
  src/GetRGBCMap.c
  src/SetRGBCMap.c
  src/GetColor.c
  src/LookupCol.c
  src/ParseCol.c
  src/AllocColor.c
  src/StColor.c
  src/StColors.c
  src/StNColor.c
  src/FreeCols.c
  src/QuColor.c
  src/QuColors.c
  src/QuBest.c
  # Cursors
  src/CrCursor.c
  src/CrGlCur.c
  src/DefCursor.c
  src/UndefCurs.c
  src/FreeCurs.c
  src/RecolorC.c
  src/QuCurShp.c
  src/Cursor.c
  # Pixmaps
  src/CrPixmap.c
  src/FreePix.c
  src/PmapBgnd.c
  src/PmapBord.c
  src/Border.c
  src/Backgnd.c
  # Input / keyboard / mouse
  src/GrKey.c
  src/GrKeybd.c
  src/UngrabKey.c
  src/UngrabKbd.c
  src/GrButton.c
  src/UngrabBut.c
  src/GrPointer.c
  src/UngrabPtr.c
  src/GrServer.c
  src/UngrabSvr.c
  src/AllowEv.c
  src/AutoRep.c
  src/KeyBind.c
  src/KeysymStr.c
  src/StrKeysym.c
  src/ModMap.c
  src/GetPntMap.c
  src/SetPntMap.c
  src/GetKCnt.c
  src/ChKeyCon.c
  src/ChPntCon.c
  src/ChClMode.c
  src/QuKeybd.c
  src/QuPntr.c
  src/WarpPtr.c
  src/SetIFocus.c
  src/GetIFocus.c
  # Misc display
  src/globals.c
  src/Misc.c
  src/Macros.c
  src/Flush.c
  src/Sync.c
  src/Synchro.c
  src/AllCells.c
  src/AllPlanes.c
  src/Bell.c
  src/PixFormats.c
  src/VisUtil.c
  src/Geom.c
  src/Depths.c
  src/ParseGeom.c
  src/ParseCmd.c
  src/DisName.c
  src/SelInput.c
  src/Pending.c
  # Screen saver
  src/SSaver.c
  src/FSSaver.c
  src/GetSSaver.c
  src/SetSSaver.c
  # Hosts
  src/Host.c
  src/LiHosts.c
  src/ChAccCon.c
  # Errors
  src/ErrHndlr.c
  src/ErrDes.c
  # Extensions
  src/InitExt.c
  src/ListExt.c
  src/QuExt.c
  # WM helpers
  src/GetHints.c
  src/SetHints.c
  src/GetNrmHint.c
  src/SetNrmHint.c
  src/GetTxtProp.c
  src/SetTxtProp.c
  src/GetWMProto.c
  src/SetWMProto.c
  src/GetWMCMapW.c
  src/SetWMCMapW.c
  src/WMProps.c
  src/WMGeom.c
  src/FetchName.c
  src/StName.c
  src/Iconify.c
  src/Withdraw.c
  src/ReconfWM.c
  src/Window.c
  src/StrToText.c
  src/TextToStr.c
  # Resources
  src/Xrm.c
  src/Quarks.c
  src/GetDflt.c
  src/ScrResStr.c
  # Regions
  src/Region.c
  src/PolyReg.c
  # Misc
  src/ChActPGb.c
  src/ChSaveSet.c
  src/DestSubs.c
  src/FreeEData.c
  src/FreeEventData.c
  src/GetEventData.c
  src/GrKeybd.c
  src/KillCl.c
  src/LiICmaps.c
  src/OCWrap.c
  src/OMWrap.c
  src/QuStipShp.c
  src/QuTileShp.c
  src/RdBitF.c
  src/RestackWs.c
  src/SetChAccCon.c
  src/Context.c
  src/udcInf.c
  src/BdrWidth.c
  src/ChCmap.c
  src/GetPCnt.c
  src/imConv.c
  src/SetLocale.c
  src/RegstFlt.c
  src/GetHColor.c
  src/XlibAsync.c
  src/xlibi18n/lcWrap.c
  src/xlibi18n/lcPublic.c
  src/xlibi18n/lcUtil.c
  src/xlibi18n/lcInit.c
  src/xlibi18n/lcGeneric.c
  src/xlibi18n/lcStd.c
  src/xlibi18n/lcUTF8.c
  src/xlibi18n/lcCharSet.c
  src/xlibi18n/lcConv.c
  src/xlibi18n/lcFile.c
  src/xlibi18n/lcDB.c
  src/xlibi18n/lcRM.c
  src/xlibi18n/lcDynamic.c
  src/xlibi18n/lcPubWrap.c
  src/xlibi18n/lcCT.c
  src/xlibi18n/lcPrTxt.c
  src/xlibi18n/lcTxtPr.c
  modules/lc/gen/lcGenConv.c
  modules/lc/def/lcDefConv.c
  modules/lc/Utf8/lcUTF8Load.c
)

# Clean old objects
rm -f *.o libX11.a

OBJS=""
for f in "${SRCS[@]}"; do
    # Skip shell redirects embedded in array
    [[ "$f" == "2>/dev/null" || "$f" == "||:" ]] && continue
    [ -f "$f" ] || { echo "  [SKIP] $f not found"; continue; }
    obj="${f//\//_}"
    obj="${obj%.c}.o"
    echo "[CC] $f -> $obj"
    $CC "${CFLAGS[@]}" -c -o "$obj" "$f" 2>&1 | grep -v "^$" || true
    OBJS="$OBJS $obj"
done

echo "[AR] Creating libX11.a..."
$AR rcs libX11.a $OBJS
echo "[OK] libX11.a built successfully."
