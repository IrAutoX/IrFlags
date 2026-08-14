import argparse
import math
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont

SIZES = (6, 8, 10, 12, 16, 24, 32, 48, 64)
FACES = ("DejaVuSansCondensedBold", "DejaVuSansMonoBold")

GLYPHS = [
    (0xFE80,),
    (0xFE81,0xFE82),
    (0xFE85,0xFE86),
    (0xFE89,0xFE8A,0xFE8B,0xFE8C),
    (0xFE8D,0xFE8E),
    (0xFE8F,0xFE90,0xFE91,0xFE92),
    (0xFB56,0xFB57,0xFB58,0xFB59),
    (0xFE95,0xFE96,0xFE97,0xFE98),
    (0xFE99,0xFE9A,0xFE9B,0xFE9C),
    (0xFE9D,0xFE9E,0xFE9F,0xFEA0),
    (0xFB7A,0xFB7B,0xFB7C,0xFB7D),
    (0xFEA1,0xFEA2,0xFEA3,0xFEA4),
    (0xFEA5,0xFEA6,0xFEA7,0xFEA8),
    (0xFEA9,0xFEAA),
    (0xFEAB,0xFEAC),
    (0xFEAD,0xFEAE),
    (0xFEAF,0xFEB0),
    (0xFB8A,0xFB8B),
    (0xFEB1,0xFEB2,0xFEB3,0xFEB4),
    (0xFEB5,0xFEB6,0xFEB7,0xFEB8),
    (0xFEB9,0xFEBA,0xFEBB,0xFEBC),
    (0xFEBD,0xFEBE,0xFEBF,0xFEC0),
    (0xFEC1,0xFEC2,0xFEC3,0xFEC4),
    (0xFEC5,0xFEC6,0xFEC7,0xFEC8),
    (0xFEC9,0xFECA,0xFECB,0xFECC),
    (0xFECD,0xFECE,0xFECF,0xFED0),
    (0xFED1,0xFED2,0xFED3,0xFED4),
    (0xFED5,0xFED6,0xFED7,0xFED8),
    (0xFB8E,0xFB8F,0xFB90,0xFB91),
    (0xFB92,0xFB93,0xFB94,0xFB95),
    (0xFEDD,0xFEDE,0xFEDF,0xFEE0),
    (0xFEE1,0xFEE2,0xFEE3,0xFEE4),
    (0xFEE5,0xFEE6,0xFEE7,0xFEE8),
    (0xFEED,0xFEEE),
    (0xFEE9,0xFEEA,0xFEEB,0xFEEC),
    (0xFBFC,0xFBFD,0xFBFE,0xFBFF),
]
PRESENTATION = [cp for forms in GLYPHS for cp in forms]
assert len(PRESENTATION) == 123


def next_pow2(value):
    return 1 << max(1, int(value - 1).bit_length())


def slot_character(slot):
    if 32 <= slot <= 126:
        return chr(slot)
    if slot == 127:
        return " "
    index = slot - 128
    if 0 <= index < len(PRESENTATION):
        return chr(PRESENTATION[index])
    return " "


def generate(ttf_path: Path, out_dir: Path, face: str, size: int):
    font = ImageFont.truetype(str(ttf_path), size=max(size, 6), layout_engine=ImageFont.Layout.BASIC)
    count = 224
    cols = 16
    rows = math.ceil(count / cols)
    cell_w = max(16, int(size * 1.65) + 10)
    cell_h = max(16, int(size * 1.9) + 10)
    tex_w = next_pow2(cols * cell_w)
    tex_h = next_pow2(rows * cell_h)
    image = Image.new("RGBA", (tex_w, tex_h), (255,255,255,0))
    draw = ImageDraw.Draw(image)
    metrics = []

    for index, slot in enumerate(range(32, 256)):
        char = slot_character(slot)
        col = index % cols
        row = index // cols
        x0 = col * cell_w
        y0 = row * cell_h
        bbox = font.getbbox(char)
        if bbox is None:
            bbox = (0,0,0,0)
        glyph_w = max(0, bbox[2] - bbox[0])
        glyph_h = max(1, bbox[3] - bbox[1])
        advance = max(1, int(round(font.getlength(char))))
        pad_x = 3
        pad_y = max(2, int(size * 0.18))
        start_x = x0 + pad_x
        start_y = y0 + pad_y
        if char != " ":
            draw.text((start_x - bbox[0], start_y - bbox[1]), char, font=font, fill=(255,255,255,255))
        char_width = glyph_w if char != " " else 0
        whitespace = advance if char == " " else max(1, advance - glyph_w)
        metrics.append((slot, 0, char_width, whitespace, start_x, start_x + glyph_w,
                        start_y, start_y + glyph_h))

    base = out_dir / f"{face}_{size}"
    image.save(base.with_suffix(".png"), optimize=True)
    with open(base.with_suffix(".fmt"), "wb") as fh:
        header = (f"NumChars: {count}\nTextureWidth: {tex_w}\nTextureHeight: {tex_h}\n"
                  f"TextZStep: {cell_h}\n\n").encode("ascii")
        fh.write(header)
        for slot, initial, width, whitespace, sx, ex, sy, ey in metrics:
            fh.write(b'Char: "' + bytes([slot]) + b'"\n')
            fh.write(f"InitialDist: {initial}\nWidth: {width}\nWhitespace: {whitespace}\n".encode("ascii"))
            fh.write(f"StartX: {sx}\nEndX: {ex}\nStartY: {sy}\nEndY: {ey}\n\n".encode("ascii"))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("ttf")
    parser.add_argument("output")
    args = parser.parse_args()
    ttf_path = Path(args.ttf)
    out_dir = Path(args.output)
    out_dir.mkdir(parents=True, exist_ok=True)
    for face in FACES:
        for size in SIZES:
            generate(ttf_path, out_dir, face, size)
    print(f"Generated {len(FACES) * len(SIZES)} Persian-capable Vazirmatn atlases in {out_dir}")


if __name__ == "__main__":
    main()
