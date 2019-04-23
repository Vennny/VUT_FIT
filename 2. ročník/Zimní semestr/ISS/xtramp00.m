pkg load signal 
%1

[Y,Fs] = audioread('xtramp00.wav');

pocetVzorku = length(Y);
cas = pocetVzorku/Fs;
pocetBinSymb = pocetVzorku / 16;

%2
hold on;
plot(1:320,Y(1:320))
for n = 8:16:320
   if Y(n)>0
     plot(n,1,'o','MarkerSize',5,'MarkerEdgeColor','red');
     line([n],[0,1]);
   elseif Y(n)<0
     plot(n,0,'o','MarkerSize',5,'MarkerEdgeColor','red');
   end
end
line([1 320], [0]);
xlabel('Cas [ms]')
xticks([0:32:320]);
xticklabels({'0','0.02', '0.04', '0.06','0.08','0.1','0.12','0.14','0.16','0.18','0.2'})
ylabel('S[n], symboly')
hold off;


%3 & 4
figure(2)
b = [0.0192, -0.0185, -0.0185, 0.0192];
a = [1, -2.8870, 2.7997, -0.9113];
ukazmito(b, a, Fs);

%4 mezni frekvence na intervalu 0 az 1000 je 500Hz, dolni propust

 
%5

filtered = filter(b,a,Y);
r = xcorr(Y,filtered);
maximum_r = max(r);
result = find(r==maximum_r);
posun = pocetVzorku-result;

%6
figure(3)
%graf z prikladu 2
hold on;
plot(1:320,Y(1:320))
line([1 320], [0]);
xlabel('Cas [ms]')
xticks([0:32:320]);
xticklabels({'0.00','0.02', '0.04', '0.06','0.08','0.10','0.12','0.14','0.16','0.18','0.20'})
ylabel('S[n], ss[n], ss_{shifted}[n], symboly')

plot(1:320, filtered(1:320), 'r')

filtered_posun = filtered(15:pocetVzorku);
plot(1:320, filtered_posun(1:320), 'y')

for k = 8:16:320
   if filtered_posun(k)>0
     plot(k,1,'o','MarkerSize',5,'MarkerEdgeColor','red');
     line([k],[0,1]);
   elseif filtered_posun(k)<0
     plot(k,0,'o','MarkerSize',5,'MarkerEdgeColor','red');
   end
end
hold off;

%7

count=1;
signal_binarni = 1:2000;
for k = 8:16:pocetVzorku
   if Y(k)>0
      signal_binarni(count) = 1;
   elseif Y(k)<0
      signal_binarni(count) = 0;
   end
   count = count + 1;
end

count=1;
posunuty_binarni = 1:1999;
for k = 8:16:pocetVzorku-posun
   if filtered_posun(k)>0
      posunuty_binarni(count) = 1;
   elseif filtered_posun(k)<0
      posunuty_binarni(count) = 0;
   end
   count = count + 1;
end

xor_vysld = xor(signal_binarni(1:1999), posunuty_binarni);

count = 0;
for k = 1:1:1999
  if xor_vysld(k) == 1
    count = count+1;
  end
end
count
chybovost = count/1999

%8
figure(4)
fourierovaTrns = fft(Y); 
vector = (0:pocetVzorku-1)*Fs/pocetVzorku; 
moduleOriginal = abs(fourierovaTrns);
hold on;
plot(vector(1:pocetVzorku/2), moduleOriginal(1:pocetVzorku/2), 'b');
xlabel('f [Hz]');
ylabel('H [z]');

fourierovaTrns = fft(filtered); 
vector = (0:pocetVzorku-1)*Fs/pocetVzorku; 
moduleOriginal = abs(fourierovaTrns);
plot(vector(1:pocetVzorku/2), moduleOriginal(1:pocetVzorku/2), 'r');
legend('s[n]','ss[n]');
hold off;

%9
figure(5)
xmin = min(min(Y)); xmax = max(max(Y));
kolik = 100;
x = linspace(xmin,xmax,kolik);

pxn = hist(Y,x) / 32000;
plot (x,pxn);

integral = trapz(x,pxn)

%10
figure(6)

Rv = xcorr(Y,'biased');
k = -pocetVzorku+1:pocetVzorku-1;

k_pocet = length(k);
pocet_cisel = 50;
zacatek = floor(k_pocet / 2 - pocet_cisel);
konec = floor(k_pocet / 2 + pocet_cisel);


plot(k(zacatek:konec), Rv(zacatek:konec))
xlabel('k')

%11
Rv0 = Rv(0 + pocetVzorku)
Rv1 = Rv(1 + pocetVzorku)
Rv16 = Rv(16 + pocetVzorku)

%12
figure(7)
x = linspace(min(Y), max(Y), 100);
[h,p,r] = hist2opt(Y(1:pocetVzorku-1), Y(2:pocetVzorku), x);
imagesc(x,x,p);
axis xy;
colorbar;

%13  2d integral shouldbe 1 and is 1
%intergral vychazi 1 !!! - potvrzeno ve funkci hist2opt

%14
disp(r);

