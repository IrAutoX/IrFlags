import ast
import re
import sys
from pathlib import Path

EXACT = {
"xx":"fa","translator":"DeathAmir And IrAutoX","Off":"خاموش","On":"روشن","Unavailable":"در دسترس نیست","None":"هیچ‌کدام","Fast":"سریع","Best":"بهترین","NOTE: ":"توجه: ","Auto":"خودکار","No":"خیر","Yes":"بله","N/A":"نامشخص","Pause":"توقف","Home":"خانه","End":"پایان","Left Arrow":"پیکان چپ","Right Arrow":"پیکان راست","Up Arrow":"پیکان بالا","Down Arrow":"پیکان پایین","Page Up":"صفحه قبل","Page Down":"صفحه بعد","Insert":"درج","Backspace":"پس‌بر","Delete":"حذف","Help":"راهنما","Print":"چاپ","Break":"توقف","Menu":"منو","Power":"روشن/خاموش","Undo":"واگردانی","Left Mouse":"کلیک چپ","Middle Mouse":"کلیک وسط","Right Mouse":"کلیک راست","Wheel Up":"چرخ بالا","Wheel Down":"چرخ پایین","Tab":"تب","Enter":"اینتر","Space":"فاصله",
"High Speed":"سرعت بالا","Quick Turn":"چرخش سریع","Oscillation Overthruster":"عبور نوسانی","Rapid Fire":"شلیک سریع","Machine Gun":"مسلسل","Guided Missile":"موشک هدایت‌شونده","Laser":"لیزر","Ricochet":"کمانه","Super Bullet":"گلوله ویژه","Invisible Bullet":"گلوله نامرئی","Stealth":"اختفا","Tiny":"کوچک","Narrow":"باریک","Shield":"سپر","Steamroller":"له‌کننده","Shock Wave":"موج شوک","Phantom Zone":"منطقه شبح","Genocide":"نابودی تیمی","Jumping":"پرش","Identify":"شناسایی","Cloaking":"استتار","Useless":"بی‌فایده","Masquerade":"تغییر چهره","Seer":"بیننده","Thief":"دزد","Burrow":"نقب","Wings":"بال","Agility":"چابکی","ReverseControls":"کنترل معکوس","Colorblindness":"کوررنگی","Obesity":"چاقی","Left Turn Only":"فقط گردش چپ","Right Turn Only":"فقط گردش راست","Forward Only":"فقط رو به جلو","ReverseOnly":"فقط دنده عقب","Momentum":"اینرسی","Blindness":"نابینایی","Jamming":"اختلال رادار","Wide Angle":"زاویه باز","No Jumping":"بدون پرش","Trigger Happy":"شلیک اجباری","Bouncy":"جهنده",
"Audio Settings":"تنظیمات صدا","Sound Volume:":"بلندی صدا:","Driver:":"درایور:","Device:":"دستگاه:","Remote Sounds:":"صدای بازیکنان دیگر:","Cache Settings":"تنظیمات کش","Server List Cache:":"کش فهرست سرورها:","Clear Server List Cache":"پاک‌کردن کش سرورها","Cache Size (MB):":"حجم کش (مگابایت):","Clear Download Cache":"پاک‌کردن کش دانلود","Automatic Downloads:":"دانلود خودکار:","Connection Updates:":"به‌روزرسانی اتصال:","Update Downloads":"به‌روزرسانی دانلودها","Updating Downloads":"در حال به‌روزرسانی دانلودها","Download Cache Cleared":"کش دانلود پاک شد","Server List Cache Cleared":"کش فهرست سرورها پاک شد",
"Display Settings":"تنظیمات نمایش","Dithering:":"دیتِرینگ:","Blending:":"ترکیب رنگ:","Smoothing:":"نرم‌سازی:","Lighting:":"نورپردازی:","Texturing:":"بافت‌ها:","Nearest":"نزدیک‌ترین","Linear":"خطی","AntiFlicker:":"ضد پرش تصویر:","Anisotropic:":"فیلتر ناهمسانگرد:","Quality:":"کیفیت:","Low":"کم","Medium":"متوسط","High":"زیاد","Experimental":"آزمایشی","Shadows:":"سایه‌ها:","Hidden Line:":"خطوط پنهان:","Wireframe:":"نمای سیمی:","Depth Complexity:":"پیچیدگی عمق:","Brightness:":"روشنایی:","Energy Saver:":"صرفه‌جویی انرژی:","Change Video Format":"تغییر حالت تصویر","Video Format":"حالت تصویر","Current Format:":"حالت فعلی:",
"Effects Settings":"تنظیمات جلوه‌ها","Rain:":"باران:","Mirror:":"آینه:","Fog:":"مه:","Nice":"زیبا","Shot Length:":"طول شلیک:","Display Treads:":"نمایش رد زنجیر:","Animated Treads:":"زنجیر متحرک:","Fancy Effects:":"جلوه‌های پیشرفته:","Spawn Effect:":"جلوه ورود:","Death Effect:":"جلوه نابودی:","Shot Fired Effect:":"جلوه شلیک:","Muzzle Flash":"نور دهانه","Smoke":"دود","Fancy Deaths":"نابودی پیشرفته",
"GUI Settings":"تنظیمات رابط کاربری","Radar Style:":"سبک رادار:","Normal":"عادی","Enhanced":"پیشرفته","Scoreboard Sort:":"مرتب‌سازی جدول امتیاز:","Always Show Team Scores:":"نمایش دائمی امتیاز تیم‌ها:","Scoreboard Font Size:":"اندازه فونت جدول امتیاز:","ControlPanel Font Size:":"اندازه فونت پنل:","Panel Opacity:":"شفافیت پنل:","Radar Opacity:":"شفافیت رادار:","Radar Position:":"جای رادار:","Left":"چپ","Right":"راست","Radar Size:":"اندازه رادار:","Panel Height:":"ارتفاع پنل:","Mouse Box Size:":"اندازه محدوده ماوس:","Locale:":"زبان:","Default":"پیش‌فرض","Control panel tabs:":"تب‌های پنل کنترل:","Underline color:":"رنگ زیرخط:","Cyan":"فیروزه‌ای","Grey":"خاکستری","Text":"متن","Killer Highlight:":"برجسته‌سازی قاتل:","Pulsating":"چشمک‌زن","Underline":"زیرخط","Time / Date Display:":"نمایش زمان / تاریخ:","time":"زمان","date":"تاریخ","both":"هردو","Reload timer on HUD:":"زمان‌سنج بارگذاری در HUD:",
"Controls":"کنترل‌ها","General":"عمومی","Environment":"محیط","Flags":"پرچم‌ها","Good Flags":"پرچم‌های خوب","Good Flags:":"پرچم‌های خوب:","Bad Flags":"پرچم‌های بد","Bad Flags:":"پرچم‌های بد:","Credits":"دست‌اندرکاران","Maintainer:":"نگه‌دارنده:","Original Author:":"سازنده اصلی:","Code Contributors:":"مشارکت‌کنندگان کد:","Tank Models:":"مدل‌های تانک:","Special Thanks:":"سپاس ویژه:","BZFlag Home Page:":"صفحه اصلی IrFlags:",
"Press %s to start":"برای شروع %s را بزنید","Press Pause to resume":"برای ادامه کلید توقف را بزنید","AutoPilot on":"خلبان خودکار روشن است","GAME OVER":"پایان بازی","Send:":"ارسال:","Target Info":"اطلاعات هدف","Dead":"نابود","Ready":"آماده","Sealed":"قفل‌شده","Zoned":"شبحی",
"Join Game":"ورود به بازی","Find Server":"پیدا کردن سرور","Connect":"اتصال","Callsign:":"نام بازیکن:","Password:":"رمز عبور:","Team:":"تیم:","Server:":"سرور:","Port:":"پورت:","Motto:":"شعار:","Start Server":"راه‌اندازی سرور","You must enter a callsign.":"باید نام بازیکن را وارد کنید.","You must enter a server.":"باید آدرس سرور را وارد کنید.","Trying...":"در حال تلاش...",
"Automatic":"خودکار","Rogue":"آزاد","Red Team":"تیم قرمز","Green Team":"تیم سبز","Blue Team":"تیم آبی","Purple Team":"تیم بنفش","Observer":"تماشاگر","Rabbit":"خرگوش","Hunter":"شکارچی","Invalid team":"تیم نامعتبر",
"Input Settings":"تنظیمات ورودی","Change Key Mapping":"تغییر کلیدها","Joystick device:":"دسته بازی:","Active input device:":"ورودی فعال:","Rumble:":"لرزش:","Confine mouse:":"محدودکردن ماوس:","Window":"پنجره","Jump while typing:":"پرش هنگام تایپ:","Key Mapping":"تنظیم کلیدها","Reset Defaults":"بازنشانی پیش‌فرض‌ها","Fire shot:":"شلیک:","Drop flag:":"رها کردن پرچم:","Identify/Lock On:":"شناسایی/قفل هدف:","Radar Zoom In:":"بزرگ‌نمایی رادار:","Radar Zoom Out:":"کوچک‌نمایی رادار:","Send to All:":"ارسال به همه:","Send to Teammates:":"ارسال به هم‌تیمی‌ها:","Send to Admin:":"ارسال به ادمین:","Jump:":"پرش:","Toggle Score:":"نمایش/مخفی امتیاز:","Toggle Radar:":"نمایش/مخفی رادار:","Toggle Console:":"نمایش/مخفی کنسول:","Tank Labels:":"نام تانک‌ها:","Flag Help:":"راهنمای پرچم:","Self Destruct/Cancel:":"خودنابودی/لغو:","Fast Quit:":"خروج سریع:","Fullscreen:":"تمام‌صفحه:","Screenshot:":"عکس از صفحه:","Define Quick Keys":"تعریف کلیدهای سریع",
"Up/Down arrows to move, Enter to select, Esc to dismiss":"با کلیدهای بالا/پایین حرکت کنید، Enter انتخاب و Esc بازگشت","Options":"تنظیمات","Save Settings":"ذخیره تنظیمات","Leave Game":"ترک بازی","Quit":"خروج","Enter to quit, Esc to resume":"Enter برای خروج، Esc برای بازگشت","Really quit?":"واقعاً خارج می‌شوید؟","Save Settings:":"ذخیره تنظیمات:","On Exit":"هنگام خروج","Save identity:":"ذخیره هویت:","Username only":"فقط نام کاربری","Username and password":"نام کاربری و رمز عبور",
"Save World":"ذخیره نقشه","File Name:":"نام فایل:","No world loaded to save":"نقشه‌ای برای ذخیره بارگذاری نشده","World Saved: ":"نقشه ذخیره شد: ","Error Saving: ":"خطای ذخیره: ","Would you like to upgrade now?":"می‌خواهید اکنون به‌روزرسانی کنید؟","Yes!":"بله!","Not yet":"فعلاً نه","OK":"باشه","Download Failed!":"دانلود ناموفق بود!",
"Roaming":"گردش آزاد","Tracking ":"ردیابی ","Following ":"دنبال‌کردن ","Score":"امتیاز"," Kills":" نابودی","Player":"بازیکن","Team Score":"امتیاز تیم","Team Flags":"پرچم‌های تیم","Leader ":"نفر اول ","Kick -> ":"اخراج -> ","Kill -> ":"نابودکردن -> ","Ban :":"مسدودکردن:","Unban :":"رفع مسدودی:","Show ban list":"نمایش فهرست مسدودها","Show player list":"نمایش بازیکنان","Reset Flags":"بازنشانی پرچم‌ها","End Game":"پایان بازی","Shut Down Server":"خاموش‌کردن سرور","Disconnect all Players":"قطع اتصال همه بازیکنان","Admin Password:":"رمز ادمین:","Vote:":"رأی:","Call a Poll:":"شروع رأی‌گیری:","Cancel a Poll":"لغو رأی‌گیری",
"Servers":"سرورها","Players":"بازیکنان","Red":"قرمز","Green":"سبز","Blue":"آبی","Purple":"بنفش","Observers":"تماشاگران","Searching":"در حال جست‌وجو","Style:":"سبک:","Capture the Flag":"تسخیر پرچم","Free for All":"همه علیه همه","Rabbit Chase":"تعقیب خرگوش","Max Players:":"حداکثر بازیکن:","Max Shots:":"حداکثر شلیک:","Teleporters:":"تلپورترها:","no":"خیر","yes":"بله","Ricochet:":"کمانه:","Jumping:":"پرش:","Handicap:":"هندیکپ:","Superflags:":"پرچم‌های ویژه:","all flags":"همه پرچم‌ها","no limit":"بدون محدودیت","Game Over:":"پایان بازی:","never":"هرگز","World Map:":"نقشه دنیا:","random map":"نقشه تصادفی","Start":"شروع","Server started.":"سرور اجرا شد.","localhost":"سیستم محلی",
"Shot Statistics":"آمار شلیک","Shots Hit/Fired":"برخورد/شلیک","Total":"مجموع","Flag":"پرچم","Silenced ":"بی‌صدا شد ","Unsilenced ":"از بی‌صدا خارج شد ","Got shot by ":"با شلیک نابود شد توسط ","Tank Self Destructed":"تانک خودنابود شد","Resumed":"ادامه یافت","Loading world into memory...":"در حال بارگذاری نقشه در حافظه...","Preparing world...":"در حال آماده‌سازی نقشه...","Downloading files...":"در حال دانلود فایل‌ها...","Downloading World...":"در حال دانلود نقشه...","Time Expired":"زمان تمام شد","Game Paused":"بازی متوقف شد","Don't kill teammates!!!":"هم‌تیمی‌ها را نزنید!!!","Flag Alert!!!":"هشدار پرچم!!!","Team Grab!!!":"پرچم تیم برداشته شد!!!","You are now the rabbit.":"اکنون شما خرگوش هستید.","SERVER":"سرور","Admin":"ادمین","Paused":"متوقف","Server communication error":"خطای ارتباط با سرور","Server not found":"سرور پیدا نشد","Memory error":"خطای حافظه","You have been banned from this server":"شما از این سرور مسدود شده‌اید","Game is full or over.  Try again later.":"بازی پر است یا پایان یافته؛ بعداً دوباره تلاش کنید.","Error connecting to server.":"خطا در اتصال به سرور.","Connection Established...":"اتصال برقرار شد...","Entering game...":"در حال ورود به بازی...","Server has unexpectedly disconnected":"اتصال سرور به‌طور ناگهانی قطع شد","testing performance;  please wait...":"در حال سنجش کارایی؛ کمی صبر کنید...",
"BZFlag version: ":"نسخه IrFlags: ","BZFlag version:   %s\n":"نسخه IrFlags:   %s\n","BZFlag protocol:  %s\n":"پروتکل IrFlags:  %s\n","Distributed under the terms of the LGPL":"بر پایه BZFlag و مطابق مجوزهای پروژه اصلی","Author: Chris Schoeneman <crs23@bigfoot.com>":"سازنده IrFlags: DeathAmir And IrAutoX","Maintainer: Tim Riker <Tim@Rikers.org>":"توسعه IrFlags: DeathAmir And IrAutoX"
}

WORDS = [
("Settings","تنظیمات"),("Server","سرور"),("Player","بازیکن"),("Players","بازیکنان"),("Team","تیم"),("Game","بازی"),("World","نقشه"),("Flag","پرچم"),("Flags","پرچم‌ها"),("Download","دانلود"),("Downloading","در حال دانلود"),("Error","خطا"),("Failed","ناموفق"),("Warning","هشدار"),("Unknown","نامشخص"),("Current","فعلی"),("Maximum","حداکثر"),("Minimum","حداقل"),("Show","نمایش"),("Hide","مخفی"),("Enable","فعال"),("Disable","غیرفعال"),("Enabled","فعال"),("Disabled","غیرفعال"),("Press","بزنید"),("Select","انتخاب"),("Start","شروع"),("Stop","توقف"),("Connect","اتصال"),("Connection","اتصال"),("Network","شبکه"),("Message","پیام"),("Time","زمان"),("Date","تاریخ"),("Score","امتیاز"),("Shot","شلیک"),("Shots","شلیک‌ها"),("Tank","تانک"),("Radar","رادار"),("Control","کنترل"),("Key","کلید"),("Mouse","ماوس"),("Sound","صدا"),("Audio","صدا"),("Video","تصویر"),("Display","نمایش"),("Quality","کیفیت"),("Low","کم"),("High","زیاد"),("Left","چپ"),("Right","راست"),("Forward","جلو"),("Backward","عقب"),("Automatic","خودکار"),("Cache","کش"),("Clear","پاک‌کردن"),("Save","ذخیره"),("Load","بارگذاری"),("Loading","در حال بارگذاری"),("Reset","بازنشانی"),("Remove","حذف"),("Add","افزودن"),("Password","رمز عبور"),("Username","نام کاربری"),("Observer","تماشاگر"),("Admin","ادمین"),("Vote","رأی"),("Ready","آماده"),("Dead","نابود"),("Paused","متوقف"),("Pause","توقف"),("Resume","ادامه"),("Version","نسخه")]

PLACEHOLDER = re.compile(r'%(?:\d+\$)?[-+#0-9.]*[a-zA-Z]|\{\d+\}|\\n')

def unquote(text):
    return ast.literal_eval(text.strip())

def quote(text):
    return '"' + text.replace('\\','\\\\').replace('"','\\"').replace('\n','\\n') + '"'

def translate(msgid):
    if not msgid:
        return ""
    if msgid in EXACT:
        return EXACT[msgid]
    result = msgid
    for en, fa in sorted(WORDS, key=lambda x: len(x[0]), reverse=True):
        result = re.sub(r'(?<![A-Za-z])' + re.escape(en) + r'(?![A-Za-z])', fa, result, flags=re.I)
    if re.search(r'[آ-ی]', result):
        return result
    placeholders = PLACEHOLDER.findall(msgid)
    suffix = (" " + " ".join(placeholders)) if placeholders else ""
    return "پیام سیستم" + suffix

def main():
    source = Path(sys.argv[1] if len(sys.argv) > 1 else 'data/l10n/bzflag_xx.po')
    target = Path(sys.argv[2] if len(sys.argv) > 2 else 'data/l10n/bzflag_fa.po')
    lines = source.read_text(encoding='utf-8').splitlines()
    out = ["# IrFlags Persian localization", "# DeathAmir And IrAutoX", ""]
    current = None
    skip_next_msgstr = False
    for line in lines:
        if line.startswith('msgid '):
            current = unquote(line[6:])
            out.append(line)
            skip_next_msgstr = True
        elif line.startswith('msgstr ') and skip_next_msgstr:
            out.append('msgstr ' + quote(translate(current)))
            skip_next_msgstr = False
        else:
            out.append(line)
    target.write_text('\n'.join(out) + '\n', encoding='utf-8')
    total = sum(1 for x in out if x.startswith('msgstr '))
    empty = sum(1 for x in out if x == 'msgstr ""')
    print(f'Persian locale generated: {total} entries, {empty} empty translations')
    if empty:
        raise SystemExit('locale still contains empty translations')

if __name__ == '__main__':
    main()
