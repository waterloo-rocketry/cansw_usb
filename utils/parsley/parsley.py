import sys
import message_types as mt

while True:
    try:
        line = sys.stdin.readline()
    except KeyboardInterrupt:
        break

    if not line:
        break

    if line.strip() == '.':
        print('.')
        continue

    try:
        msg_sid = int(line.split(':')[0], 16)
        msg_data_raw = line.split(':')[1].split(',')
        msg_data = [int(byte, 16) for byte in msg_data_raw]

        # lol @ bitwise manips in python
        msg_type = mt.msg_type_str[msg_sid & 0x7e0]
        board_id = mt.board_id_str[msg_sid & 0x1f]

        header = '[ ' + msg_type + ', ' + board_id + ' ] '

        # goddawful brute force message parser by message type
        if (msg_type == 'GENERAL_CMD'):
            timestamp = msg_data[0] << 16 | msg_data[1] << 8 | msg_data[2]
            cmd = mt.gen_cmd_str[msg_data[3]]
            print(header + 't=' + str(timestamp) + 'ms ' + cmd)

        elif (msg_type == 'VENT_VALVE_CMD' or msg_type == 'INJ_VALVE_CMD'):
            timestamp = msg_data[0] << 16 | msg_data[1] << 8 | msg_data[2]
            valve_state = mt.valve_states_str[msg_data[3]]
            print(header + 't=' + str(timestamp) + 'ms ' + valve_state)

        elif (msg_type == 'DEBUG_MSG'):
            # not suppoted yet
            print(line)

        elif (msg_type == 'DEBUG_PRINTF'):
            # not supported yet
            print(line)

        elif (msg_type == 'VENT_VALVE_STATUS' or msg_type == 'INJ_VALVE_STATUS'):
            timestamp = msg_data[0] << 16 | msg_data[1] << 8 | msg_data[2]
            valve_state = mt.valve_states_str[msg_data[3]]
            req_valve_state = mt.valve_states_str[msg_data[4]]
            print(header + 't=' + str(timestamp) + 'ms REQ: '+ req_valve_state + ', ACTUAL: ' + valve_state)

        # I'm sorry
        elif (msg_type == 'GENERAL_BOARD_STATUS'):
            timestamp = msg_data[0] << 16 | msg_data[1] << 8 | msg_data[2]
            board_stat = mt.board_stat_str[msg_data[3]]
            super_header = header + 't=' + str(timestamp) + 'ms ' + board_stat

            if board_stat == 'E_NOMINAL':
                # nothing else to add lol
                print(super_header)

            elif board_stat == 'E_BUS_OVER_CURRENT':
                current = msg_data[4] << 8 | msg_data[5]
                print(super_header + ' ' + current + ' mA')

            elif board_stat == 'E_BUS_UNDER_VOLTAGE' \
                or board_stat == 'E_BUS_OVER_VOLTAGE' \
                or board_stat == 'E_BATT_UNDER_VOLTAGE' \
                or board_stat == 'E_BATT_OVER_VOLTAGE':
                voltage = msg_data[4] << 8 | msg_data[5]
                print(super_header + ' ' + current + ' mV')

            elif board_stat == 'E_BOARD_FEARED_DEAD' \
                or board_stat == 'E_MISSING_CRITICAL_BOARD':
                print(super_header + ' ' + board_id_str[msg_data[4]])

            elif board_stat == 'E_NO_CAN_TRAFFIC' \
                or board_stat == 'E_RADIO_SIGNAL_LOST':
                print(super_header + ' ' + str((msg_data[4] << 8 | msg_data[5])) + 'ms')

            elif board_stat == 'E_VALVE_STATE':
                print(super_header + ' ' + 'currently not parsed')

            elif board_stat == 'E_CANNOT_INIT_DACS':
                print(super_header)

            elif board_stat == 'E_VENT_POT_RANGE':
                print(super_header + ' ' + 'currently not parsed')

            elif board_stat == 'E_SENSOR':
                print(super_header + ' ' + mt.sensor_id_str[msg_data[4]])

            elif board_stat == 'E_GPS':
                print(super_header + ' yay GPS')
            # all the stuff I don't really care about
            elif board_stat == 'E_LOGGING' \
                or board_stat == 'E_ILLEGAL_CAN_MSG' \
                or board_stat == 'E_SEGFAULT' \
                or board_stat == 'E_UNHANDLED_INTERRUPT' \
                or board_stat == 'E_CODING_FUCKUP':
                print(super_header)


        elif (msg_type == 'SENSOR_ACC' \
            or msg_type == 'SENSOR_GYRO' \
            or msg_type == 'SENSOR_MAG'):
            # not supported yet
            print(line)

        elif (msg_type == 'SENSOR_ANALOG'):
            timestamp = msg_data[0] << 8 | msg_data[1]
            sensor_id = mt.sensor_id_str[msg_data[2]]
            value = msg_data[3] << 8 | msg_data[4]
            print(header + 't=' + str(timestamp) + 'ms ' + sensor_id + ' ' + str(value))

        elif (msg_type == 'GPS_TIMESTAMP'):
            utc_hours = msg_data[3]
            utc_mins = msg_data[4]
            utc_secs = msg_data[5]
            utc_dsecs= msg_data[6]
            print('hours=' + utc_hours + ' mins=' + utc_mins + ' seconds=' +utc_secs + '.' + utc_dsecs)

        elif (msg_type == 'GPS_LATITUDE'):
            degrees = msg_data[3]
            minutes = msg_data[4]
            dminutes = msg_data[5]
            direction = msg_data[6]
            print('Latitude=' + degrees + ' ' + minutes + '.' + dminutes + ' ' + direction)

        elif (msg_type == 'GPS_LONGITUDE'):
            degrees = msg_data[3]
            minutes = msg_data[4]
            dminutes = msg_data[5]
            direction = msg_data[6]
            print('Longitude=' + degrees + ' ' + minutes + '.' + dminutes + ' ' + direction)

        elif (msg_type == 'GPS_ALTITUDE'):
            altitude = msg_data[3] << 8 | msg_data[4]
            daltitude = msg_data[5]
            unit = msg_data[6]
            print('Altitude=' + altitude + '.' + daltitude + ' ' + unit)
        
        elif(msg_type == 'GPS_INFO'):
            numsat = msg_data[3]
            quality = msg_data[4]
            print('Number of satellites=' + numsat + '. Quality=' + quality)

        elif (msg_type == 'LEDS_ON'):
            print(header)

        elif (msg_type == 'LEDS_OFF'):
            print(header)

        else:
            print('Message type not known, original message: ' + line)

    except:
        print('Unable to parse message: ' + line)
        continue
