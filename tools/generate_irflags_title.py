import sys
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont

font_path = Path(sys.argv[1])
out_path = Path(sys.argv[2] if len(sys.argv) > 2 else 'data/title.png')
canvas = Image.new('RGBA', (1024, 256), (0, 0, 0, 0))
draw = ImageDraw.Draw(canvas)
font = ImageFont.truetype(str(font_path), 132)
sub = ImageFont.truetype(str(font_path), 28)
text = 'IrFlags'
b = draw.textbbox((0, 0), text, font=font, stroke_width=2)
x = (canvas.width - (b[2]-b[0])) // 2
y = 20
for offset, alpha in ((10, 35), (6, 60), (3, 100)):
    draw.text((x, y + offset), text, font=font, fill=(0,0,0,alpha), stroke_width=2, stroke_fill=(0,0,0,alpha))
draw.text((x, y), text, font=font, fill=(245,245,245,255), stroke_width=2, stroke_fill=(35,150,70,255))
subtext = 'DeathAmir And IrAutoX'
b2 = draw.textbbox((0,0), subtext, font=sub)
x2 = (canvas.width - (b2[2]-b2[0])) // 2
draw.text((x2, 194), subtext, font=sub, fill=(225,225,225,235))
out_path.parent.mkdir(parents=True, exist_ok=True)
canvas.save(out_path, optimize=True)
print(out_path)
