% Script to update detection data from Arduino to ThingSpeak every five
% minutes
% Author: Ryotaro Hokao

% Channel ID and APIkeys
% channelID = 2026263;
% writeAPIKey = 'WN6UZKY58ABMJW89';  
% readAPIKey = 'AQRWX7EYH43YYRGD'; 

% API info for my personal ThingSpeak channel (for debugging)
channelID = 2220888;
writeAPIKey = 'NODGSWNS9MDPIWDW';  
readAPIKey = '7EY0JOIIZY4DT9DU';

% Variables
infmt = "yyyy-MM-dd'T'HHmmss"; % input format for converting string timestamp to datetime object

% Read from Arduino-dedicated field on ThingSpeak channel
[data_from_Arduino,read_tStamps] = thingSpeakRead(channelID,'Fields',6,NumPoints=15,ReadKey=readAPIKey);
% Check if there are any newer data to be updated 
if (sum(data_from_Arduino,"omitmissing") > 0) 
    % there are non-NaN data in the array
    all_index = find(~isnan(data_from_Arduino)); % array of indices with non-NaN value
    % ToDo: ignore those counter values that probably are meaningless 
    
    % Decode and format the time of collision
    tStamps_in_string = strings(length(all_index),1); % preallocate string array for timestamp
    for i = 1:length(all_index)
        % d_in_string = string(datetime('now','Format','yyyy-MM-dd')); % get formatted today's date in string
        d_in_string = string(datetime('2023-07-17','Format','yyyy-MM-dd')); % date for debugging 
        t_in_string = data_from_Arduino(all_index(i)); % get time in string
        dt_in_string = d_in_string + 'T' + t_in_string; % format datetime in ISO8601 format "yyyy-MM-ddTHHmmss"
        tStamps_in_string(i) = dt_in_string;
    end
    tStamps = datetime(tStamps_in_string,'InputFormat',infmt); % convert string array to datetime array
    tStamps = rmmissing(tStamps); % remove NaT values

    % Compare with the collision data filed on ThingSpeak channel to make sure
    % there are no duplicates
    uploaded_collision = thingSpeakRead(channelID,'Fields',5,'DateRange',[min(tStamps),max(tStamps)],'OutputFormat','timetable',ReadKey=readAPIKey); % fetch extra data to compare
    uploaded_collision = rmmissing(uploaded_collision); % remove NaN entries (i.e., data entries to other ThingSpeak fields)
    to_delete = nan(length(tStamps),1); % preallocate array that indicates row(s) to delete
    for i = 1:length(tStamps)
        x = tStamps(i);
        for j = 1:height(uploaded_collision)
            if (x == uploaded_collision.Timestamps(j))
                % If a timestamp in tStamps matches with the timestamp of already-uploaded collision, 
                % that's a duplicate data and shouldn't be written to thingSpeak channel (because that will result in an error)
                to_delete(i) = i; 
                break
            end
        end
    end
    to_delete = rmmissing(to_delete); % remove NaN 
    if (isempty(to_delete))
        % no duplicate data, safe to upload
    else 
        final_tStamps = tStamps;
        final_tStamps(to_delete,:) = []; % remove duplicate timestamps
    end

    % Write new data point to the collision data field
    collision = ones(length(final_tStamps),1); 
    % thingSpeakWrite(channelID,'Fields',5,'Values',collision,'Timestamp',final_tStamps,'WriteKey',writeAPIKey);
else 
    % there are no collision data to write to ThingSpeak channel
    % ToDo: get current time and write 0 (?)
end 