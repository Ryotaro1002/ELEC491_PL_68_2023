% Read outside and inside surface temperatures every 30 minutes from separate data fields within one channel;
% Based on received temperature data, default or manually changed R-value and the size of window, calculate the heat transfer;
% Write the calculated result to another data field.

% Changeable Constants for the heat transfer calculation
U_value = 1.2;
Window_Height = 0.7;    % Unit(meter)
Window_Width = 0.8;     % Unit(meter)

% Do not modify the code below ------------------------------------------------------
%
%
%
%
%
%
%
%
%
% Do not modify the code below ------------------------------------------------------

R_value = (1/U_value); % R value is the reciprocal of U value or U factor

% Channel ID and APIkey
ChannelID = 2026263; 
readAPIKey = 'AQRWX7EYH43YYRGD';
writeAPIKey = 'WN6UZKY58ABMJW89'; 

% Field IDs  
inside_surface_temp_FieldID = 1;
outside_surface_temp_FieldID = 2;
heat_transfer_FieldID = 4;

% Read inside and outside surface temperatures from the data field 1 and 2 seperately
data = thingSpeakRead(ChannelID,'Fields',[1,2],NumPoints=10,ReadKey = readAPIKey);
inside_surface_temp = data(10,1);
outside_surface_temp = data(10,2);

% Skip NaNs in data arrays until capture updated temperature data
if isnan(inside_surface_temp) || isnan(outside_surface_temp) 
    x = 10;
    y = 10;
    while isnan(inside_surface_temp)
        x = x-1;
        inside_surface_temp = data(x,1);
    end
    
    while isnan(outside_surface_temp) 
        y = y-1;
        outside_surface_temp = data(y,2);
    end
end

% Calculate the heat flux and heat transfer 
difference_in_temperature = abs(inside_surface_temp - outside_surface_temp);
Heat_flux = difference_in_temperature / R_value;
Heat_transfer = Heat_flux * Window_Height * Window_Width;
    
display(inside_surface_temp,'inside_surface_temp');
display(outside_surface_temp,'outside_surface_temp');
display(Heat_transfer,'Heat_transfer');

% Write the heat transfer calculated into the data field 4 to store and display
thingSpeakWrite(ChannelID,'Fields',4,'Values',Heat_transfer,'WriteKey',writeAPIKey);
