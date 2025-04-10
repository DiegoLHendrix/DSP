pinchSegment = outData(290:400,:);
plot(pinchSegment(:,1), pinchSegment(:,2))
grid on

coefficients = polyfit(pinchSegment, signal, 1);
slope = coefficients(1);
intercept = coefficients(2);