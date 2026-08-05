# Wireless and Antenna System — ESP32-2432S028

> **Document status:** sample-specific wireless and antenna reference for the author's `ESP32-2432S028` board.  
> **Scope:** ESP-WROOM-32 radio capabilities, module PCB antenna, placement and enclosure effects, Wi-Fi and Bluetooth operating modes, power integrity, coexistence, security, throughput, range, diagnostics, and verification.  
> **Evidence rule:** visible module identity and repository behavior are separated from module-family documentation, software-stack behavior, RF inference, and unmeasured board performance.

![ESP-WROOM-32 macro photograph](../../hardware/images/esp-wroom-32.jpg)

## 1. Purpose

The board's wireless subsystem is physically compact but system-wide in its effects.

It includes:

- the radio section inside the ESP-WROOM-32 module;
- the module's integrated PCB antenna;
- the carrier-board placement around the antenna end;
- the 3.3-V supply path that supports radio-current bursts;
- firmware configuration for Wi-Fi and Bluetooth;
- the enclosure, display, cables, speaker, power source and nearby objects that alter RF behavior.

A successful network connection proves only that the radio can associate under one set of conditions. It does not establish:

- maximum range;
- stable throughput;
- antenna efficiency;
- enclosure compatibility;
- Bluetooth coexistence;
- low-noise power delivery;
- production repeatability;
- regulatory suitability of a modified installation.

## 2. Evidence summary

| Item | Current status | Evidence | Still open |
|---|---|---|---|
| ESP-WROOM-32 module | Photo-confirmed | readable module marking | exact internal silicon revision and Flash population |
| Integrated PCB antenna | Physically visible as module antenna end | module construction and board photograph | measured radiation pattern, efficiency and detuning |
| 2.4-GHz Wi-Fi capability | Module-family documented | ESP-WROOM-32 family | exact stack features and qualified performance |
| Bluetooth Classic and BLE capability | Module-family documented | classic ESP32 module family | repository examples and board-level coexistence tests |
| Current Wi-Fi station example | Repository-confirmed | `examples/04_wifi_clock/04_wifi_clock.ino` | RSSI, reconnect and long-term reliability |
| 20-second connection deadline | Repository-confirmed | Wi-Fi clock example | suitability for deployment environment |
| NTP time synchronization | Repository-confirmed | `pool.ntp.org`, `time.nist.gov` | DNS, captive portal and offline behavior |
| Antenna keep-out requirement | Strong module-family rule | module antenna construction | actual enclosure clearance and board surroundings |
| Range, throughput and packet loss | Open | no recorded RF qualification | full measurement campaign |
| Country/channel configuration | Open | no sample-specific record | deployed regulatory domain and access-point plan |

## 3. Wireless subsystem block view

```text
Application firmware
      |
Wi-Fi / Bluetooth stack
      |
ESP32 radio, baseband and coexistence logic
      |
module RF network
      |
integrated PCB antenna
      |
2.4-GHz propagation environment
      |
access point, phone, gateway or peer device
```

Power and mechanics intersect this path:

```text
USB / external power
      |
3.3-V regulation and decoupling
      |
radio current bursts

Enclosure / display / cables / metal / human body
      |
antenna detuning, absorption and pattern distortion
```

## 4. Confirmed module identity

The photographed board carries an **ESP-WROOM-32** module.

This identifies the classic ESP32 module family rather than ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C6 or another later family.

The module integrates:

- the ESP32 SoC;
- RF matching and support circuitry;
- reference clocks;
- module program Flash;
- metal RF shield;
- a PCB antenna at the unshielded module end.

The carrier-board documentation must retain this exact observed module identity. Seller descriptions for a different ESP32 family must not silently replace it.

## 5. Module-family wireless capabilities

### 5.1 Wi-Fi band

The classic ESP-WROOM-32 family operates in the 2.4-GHz Wi-Fi band.

It is not a 5-GHz Wi-Fi module.

A dual-band access point must therefore provide a compatible 2.4-GHz network if the board is expected to connect.

### 5.2 Wi-Fi standards

The module family supports the classic 2.4-GHz IEEE 802.11 b/g/n feature set described by Espressif documentation.

Actual behavior also depends on:

- framework and SDK version;
- selected channel width and access-point configuration;
- security mode;
- regional settings;
- coexistence with Bluetooth;
- power-save mode;
- application load.

### 5.3 Bluetooth

The classic ESP32 family includes:

- Bluetooth Classic capability;
- Bluetooth Low Energy capability.

The current repository does not yet provide a Bluetooth-specific board example or a sample-level Bluetooth qualification record.

Therefore Bluetooth support is documented as a module-family capability, not as a verified application path for this board sample.

## 6. Integrated PCB antenna

### 6.1 Physical location

The antenna is located at the end of the ESP-WROOM-32 module where the metal shield stops and the patterned PCB region remains exposed.

This end should be treated as the RF-sensitive edge of the board.

### 6.2 Functional role

The PCB antenna converts RF current in the module feed structure into radiated electromagnetic energy and receives energy from the surrounding field.

Its performance depends on more than the visible antenna trace. It also depends on:

- module ground reference;
- carrier-board copper near the antenna;
- nearby dielectric materials;
- conductive objects;
- orientation relative to the peer antenna;
- installed enclosure.

### 6.3 No external antenna connector is claimed

The photographed sample does not establish the presence of a user-accessible external RF connector.

Do not assume that an unpopulated pad, test point or small component is an antenna connector or selectable RF path.

Any RF modification requires schematic-level confirmation and regulatory review.

## 7. Antenna keep-out

### 7.1 Conservative keep-out rule

Keep the antenna end clear of:

- metal enclosure walls;
- batteries with metallic foil or cans;
- loudspeaker frames and magnets;
- large ground planes added by another PCB;
- shielded cables;
- USB cables folded over the antenna;
- wiring bundles;
- mounting brackets;
- conductive adhesive or foil;
- heat spreaders;
- the user's hand during fixed-position testing.

### 7.2 Carrier-board copper

The module is already mounted on a carrier PCB whose layout is fixed. Additional user hardware should not extend conductive material into the antenna region without measurement.

### 7.3 Vertical clearance

Objects above and below the antenna can matter, not only objects beside it.

A metal display bracket, rear cover or battery directly behind the antenna can reduce performance even if no object touches the antenna trace.

### 7.4 Cable routing

Route power, speaker and sensor cables away from the antenna end.

If a cable must pass nearby:

1. fix its position mechanically;
2. test the intended length and orientation;
3. repeat RSSI and throughput measurements;
4. test with cable current representative of real use;
5. document the installed geometry.

## 8. Board orientation and polarization

The integrated antenna has a non-isotropic pattern. Board orientation can change link quality.

Test at least:

- display facing the access point;
- PCB side facing the access point;
- antenna edge toward the access point;
- antenna edge away from the access point;
- board horizontal;
- board vertical;
- intended final installation angle.

Do not choose an enclosure orientation from one RSSI reading. Use repeated packet and throughput measurements.

## 9. Enclosure effects

### 9.1 Plastic enclosure

Plastic is often less disruptive than metal, but it can still detune the antenna depending on:

- material permittivity;
- wall thickness;
- pigments and fillers;
- moisture;
- distance from the antenna;
- internal metal fasteners.

### 9.2 Metal enclosure

A closed metal enclosure can severely attenuate 2.4-GHz communication.

A plastic window or deliberate RF opening may be required, but its effectiveness must be tested in the complete assembly.

### 9.3 Conductive coatings

EMI coatings, metallic paint and foil labels can behave more like metal than ordinary plastic.

### 9.4 Fasteners and standoffs

Metal standoffs near the antenna can distort the radiation pattern. Prefer nonconductive hardware near the RF edge when mechanically suitable.

### 9.5 Display assembly

The TFT assembly contains conductors and occupies most of one side of the board. The display orientation and any added bezel or backing plate can affect RF performance.

The final system must be tested with the actual display, bezel and rear cover installed.

## 10. Human-body and installation effects

The human body absorbs and perturbs 2.4-GHz fields.

Handheld tests can differ from wall-mounted or desktop operation.

Record whether measurements were made:

- held in hand;
- placed on a wooden surface;
- placed on a metal bench;
- mounted in the intended product;
- connected to a laptop by USB;
- powered from a free-standing supply.

## 11. Current repository Wi-Fi example

The repository contains `examples/04_wifi_clock/04_wifi_clock.ino`.

Its relevant behavior is:

```cpp
WiFi.mode(WIFI_STA);
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
```

The example:

- operates in station mode;
- waits up to approximately 20 seconds for connection;
- displays a failure status if connection is not established;
- requests time from `pool.ntp.org` and `time.nist.gov`;
- displays synchronized time on the TFT.

This proves the intended software path, not sample-level RF performance.

## 12. Credential handling

The example includes:

```cpp
#include "secrets.h"
```

Credentials should remain outside committed source files.

Do not publish:

- real SSIDs when privacy matters;
- Wi-Fi passwords;
- enterprise credentials;
- private keys;
- cloud tokens;
- unique device provisioning secrets.

A safe repository pattern is:

1. commit a template such as `secrets.example.h`;
2. ignore the real `secrets.h`;
3. document required symbols;
4. avoid printing passwords to Serial;
5. rotate credentials if accidentally committed.

## 13. Wi-Fi operating modes

### 13.1 Station mode

Station mode connects the board to an existing access point.

Suitable uses include:

- NTP time;
- HTTP or MQTT clients;
- local dashboards;
- telemetry upload;
- remote control.

### 13.2 Soft access point mode

SoftAP mode lets phones or computers connect directly to the board.

Potential uses:

- initial provisioning;
- local configuration;
- maintenance interface;
- isolated demonstrations.

The repository has not yet qualified SoftAP range, client count or stability.

### 13.3 Combined station and access point mode

Combined operation can be useful for provisioning or maintenance, but it increases complexity and may affect airtime, memory and power behavior.

### 13.4 Scanning

Network scans can reveal:

- SSID;
- BSSID;
- channel;
- RSSI;
- advertised security mode.

Scanning also consumes time and power and can disrupt application timing.

## 14. Access-point compatibility

Test the intended access point with:

- 2.4-GHz radio enabled;
- compatible security mode;
- visible and hidden SSID cases if required;
- DHCP availability;
- sufficient address lease duration;
- no captive portal unless supported;
- no unsupported enterprise-authentication assumptions;
- channel settings allowed in the deployment region.

A board can associate yet fail at DNS, DHCP, gateway or Internet access.

## 15. Connection-state decomposition

Record the network process as separate stages:

```text
radio enabled
network found
association attempted
authentication completed
IP address obtained
gateway reachable
DNS works
application server reachable
application protocol succeeds
```

A generic “Wi-Fi failed” message hides the fault location.

## 16. RSSI interpretation

RSSI is useful as a relative link indicator, but it is not a complete performance metric.

RSSI depends on:

- board orientation;
- multipath;
- access-point transmit power;
- antenna design at both ends;
- channel interference;
- enclosure;
- time and movement.

Two links with similar RSSI can have different packet loss and throughput.

### 16.1 Measurement practice

For each location:

1. allow the link to stabilize;
2. record many RSSI samples;
3. report median, minimum and maximum;
4. record channel and BSSID;
5. measure packet loss and throughput;
6. repeat with the final enclosure;
7. repeat at different times if interference varies.

## 17. Range qualification

### 17.1 Indoor route

Define fixed test points:

```text
R0: same room, line of sight
R1: same room, intended orientation
R2: one wall
R3: two walls
R4: farthest required indoor point
```

### 17.2 Outdoor route

For outdoor testing, define:

- access-point height;
- board height;
- line-of-sight condition;
- weather;
- orientation;
- movement speed if mobile;
- legal and safe test area.

### 17.3 Pass criteria

A useful range pass condition should include:

- successful connection within a declared time;
- bounded packet loss;
- minimum sustained throughput;
- acceptable latency;
- recovery after brief obstruction;
- no brownout or watchdog reset.

## 18. Throughput testing

### 18.1 Separate directions

Measure:

- board-to-host TCP;
- host-to-board TCP;
- board-to-host UDP;
- host-to-board UDP.

### 18.2 Report conditions

Record:

```text
Board sample:
Firmware and framework version:
Access point:
Channel:
Channel width:
Security:
Distance:
Orientation:
Enclosure:
Power source:
Wi-Fi power-save setting:
Bluetooth active/inactive:
TFT update rate:
SD activity:
Audio activity:
TCP/UDP:
Direction:
Duration:
Throughput:
Packet loss:
Latency:
Resets:
```

### 18.3 Application throughput

Application protocols add overhead.

Measure real workloads such as:

- MQTT messages per second;
- HTTP response time;
- image download time;
- telemetry batch transfer;
- OTA image transfer;
- WebSocket update latency.

## 19. Latency and packet loss

Average latency alone can hide stalls.

Report:

- minimum;
- median;
- 95th percentile;
- 99th percentile;
- maximum;
- loss percentage;
- longest outage;
- reconnect count.

Interactive display control may be more affected by jitter than by average throughput.

## 20. Long-duration reliability

Run staged tests:

1. 10-minute functional test;
2. one-hour throughput and reconnect test;
3. overnight idle connection;
4. overnight periodic telemetry;
5. repeated access-point reboot test;
6. repeated DHCP renewal test;
7. repeated board brownout recovery test only under controlled conditions.

Log:

- connection events;
- disconnect reasons when available;
- IP changes;
- RSSI trend;
- free heap;
- largest free block if relevant;
- watchdog and reset reasons;
- application queue depth;
- NTP resynchronization.

## 21. Reconnection strategy

A robust application should not block forever in a tight connection loop.

Recommended behavior:

- use a finite initial deadline;
- keep the user interface responsive;
- retry with bounded backoff;
- distinguish wrong credentials from temporary absence;
- avoid rapid continuous scanning;
- preserve offline functionality where possible;
- report state without exposing secrets;
- recover after access-point restart.

The current Wi-Fi clock uses a 20-second startup deadline and then returns from setup on failure. That is suitable as a simple example but not a complete long-term reconnection manager.

## 22. NTP and time synchronization

The Wi-Fi clock calls:

```cpp
configTime(GMT_OFFSET_SECONDS,
           DAYLIGHT_OFFSET_SECONDS,
           "pool.ntp.org",
           "time.nist.gov");
```

Successful Wi-Fi association does not guarantee NTP success.

NTP may fail because of:

- DNS failure;
- blocked UDP traffic;
- captive portal;
- no Internet route;
- invalid server response;
- time-service outage;
- application timeout;
- network not yet fully ready.

Applications should distinguish:

```text
Wi-Fi connected
IP acquired
DNS resolved
NTP synchronized
clock remains valid
```

## 23. Wi-Fi security

### 23.1 Network authentication

Supported security behavior depends on the ESP32 software stack and access-point configuration.

Do not document a security mode as supported merely because the access point offers it.

Test the exact combination used in deployment.

### 23.2 Transport security

Wi-Fi link encryption does not replace end-to-end application security.

For sensitive data, consider:

- TLS server authentication;
- certificate validation;
- secure credential storage;
- replay resistance;
- authenticated update process;
- limited local services;
- safe failure when time is not synchronized.

### 23.3 Open access points

Avoid transmitting secrets over an open network without end-to-end encryption.

### 23.4 Provisioning

Provisioning interfaces should not remain permanently open or unauthenticated after setup unless that is an explicit controlled design decision.

## 24. MAC addresses and privacy

Wi-Fi and Bluetooth interfaces have unique identifiers.

Do not publish full device identifiers in public logs unless needed.

Test records can use a redacted form such as:

```text
AA:BB:CC:xx:xx:xx
```

When stable identity is needed for fleet management, store it in controlled records.

## 25. Country, channel and regulatory domain

Wireless channel availability and transmit behavior are subject to regional rules.

The deployment must use:

- the correct country configuration;
- channels permitted in the installation region;
- certified module operating conditions;
- an enclosure and antenna arrangement consistent with applicable requirements.

Do not increase transmit power, alter the module RF path or attach an unofficial external antenna without engineering and regulatory review.

## 26. Power integrity during radio activity

### 26.1 Burst current

Radio transmit and receive activity creates dynamic current demand.

A supply can show an acceptable average voltage while suffering short droops that cause:

- brownout reset;
- disconnect;
- packet loss;
- corrupted peripheral operation;
- audio noise;
- TFT artifacts;
- SD errors.

### 26.2 Measurement points

Capture:

- USB VBUS at the connector;
- board 5-V node;
- both AMS1117 outputs;
- voltage near the ESP-WROOM-32 supply pins if accessible;
- EN and reset reason;
- current at the power source.

### 26.3 Test loads

Measure radio behavior with:

1. TFT backlight off;
2. TFT backlight full;
3. RGB LED active;
4. SD writing;
5. touch sampling;
6. audio idle;
7. audio output into the intended speaker;
8. external CN1 load;
9. USB direct;
10. intended cable and hub.

### 26.4 Decoupling modifications

Do not add capacitors blindly.

Before modifying:

- identify rail topology;
- measure the transient;
- confirm regulator stability requirements;
- select low-ESR parts appropriately;
- check startup and inrush behavior;
- document the modification.

## 27. Regulator thermal interaction

The board contains two AMS1117-3.3 regulators, but their load division remains unverified.

Wi-Fi testing should record:

- regulator case temperature;
- ambient temperature;
- enclosure state;
- radio duty cycle;
- TFT, SD and audio loads;
- board resets or throttling symptoms.

A closed enclosure can reduce both RF performance and thermal margin.

## 28. Wi-Fi and Bluetooth coexistence

Wi-Fi and Bluetooth share the 2.4-GHz radio resources of the classic ESP32 platform.

Concurrent use can affect:

- Wi-Fi throughput;
- Bluetooth latency;
- packet loss;
- scan timing;
- audio continuity;
- current consumption.

Do not assume independent simultaneous full performance.

### 28.1 Coexistence test matrix

| Wi-Fi load | Bluetooth load | TFT | SD | Audio | Record |
|---|---|---|---|---|---|
| idle associated | BLE advertising | static | idle | off | current, RSSI, stability |
| TCP upload | BLE advertising | active | idle | off | throughput, packet loss |
| TCP download | BLE connection | active | write | off | latency, resets |
| MQTT telemetry | Bluetooth Classic link | active | log | active | dropouts, audio artifacts |
| reconnect loop | BLE scan | active | idle | off | connection time, watchdogs |

Bluetooth Classic scenarios apply only after a suitable repository example and peer device are defined.

## 29. TFT interaction

The display does not share the RF medium, but it can influence the wireless subsystem through:

- supply current;
- digital switching noise;
- CPU and bus load;
- enclosure geometry;
- metal content in the display assembly.

Test wireless performance with:

- static screen;
- continuous full-screen redraw;
- partial updates;
- backlight off and full;
- DMA or non-DMA transfer if used.

## 30. microSD interaction

SD activity can affect Wi-Fi through:

- current transients;
- CPU scheduling;
- heap usage;
- interrupt latency;
- logging stalls.

A network logger should be tested for:

- simultaneous receive and file write;
- file flush pauses;
- card removal handling;
- backlog growth during reconnect;
- data duplication after retry;
- power loss during active radio and storage operations.

## 31. Audio interaction

The audio amplifier and speaker can interact with radio testing through:

- supply current;
- electromagnetic noise;
- speaker wiring near the antenna;
- metal speaker frame or magnet;
- CPU load from waveform generation.

Keep speaker wiring away from the antenna and repeat RF tests with the intended speaker installed.

## 32. USB-cable effect

A USB cable is a long conductive object connected to the board ground and power system.

Its routing can alter:

- antenna pattern;
- common-mode currents;
- noise coupling;
- orientation repeatability.

Compare:

- USB-powered and cable-connected operation;
- battery or isolated supply operation if safely available;
- cable routed toward the antenna;
- cable routed away from the antenna;
- intended production cable.

## 33. Access-point placement

Wireless performance is a two-ended system.

Record access-point:

- model and firmware;
- antenna orientation;
- mounting height;
- transmit-power setting;
- channel;
- channel width;
- nearby interference;
- distance and obstacles.

Do not attribute every poor result to the board antenna.

## 34. Channel interference survey

The 2.4-GHz band may contain:

- neighboring Wi-Fi networks;
- Bluetooth devices;
- microwave-oven emissions;
- wireless cameras;
- proprietary 2.4-GHz links;
- USB 3-related noise in some installations;
- industrial interference.

Record channel occupancy and repeat tests on a controlled channel where possible.

## 35. Basic Wi-Fi diagnostic firmware

A sample diagnostic should report:

```text
firmware version
framework version
Wi-Fi mode
connection state
SSID, optionally redacted
BSSID, redacted if public
channel
RSSI
IP address
subnet
gateway
DNS server
connect duration
disconnect count
last disconnect reason
free heap
reset reason
```

It should:

- avoid printing the password;
- use bounded retries;
- continue displaying status offline;
- timestamp events after time becomes valid;
- preserve early events with uptime timestamps.

## 36. Wi-Fi scan test

Perform a scan at fixed locations and record:

| Location | Orientation | Enclosure | SSID/BSSID | Channel | RSSI | Networks found | Notes |
|---|---|---|---|---:|---:|---:|---|
| R0 | antenna edge toward AP | open | redacted | | | | |
| R0 | antenna edge away | open | redacted | | | | |
| R0 | intended orientation | closed | redacted | | | | |
| R2 | intended orientation | closed | redacted | | | | |

Scanning should not be used as the only range test.

## 37. TCP and UDP test plan

### 37.1 TCP

Measure:

- connection establishment time;
- sustained upload and download;
- retransmission-related stalls;
- recovery after access-point interruption;
- memory behavior with large buffers.

### 37.2 UDP

Measure:

- offered packet rate;
- received packet rate;
- loss;
- reordering;
- jitter;
- burst behavior.

### 37.3 Payload sizes

Test multiple payload sizes rather than one favorable value.

Example sequence:

```text
64 B
256 B
1024 B
4096 B
application-realistic payload
```

## 38. Application-service testing

### 38.1 HTTP client

Record:

- DNS time;
- TCP connect time;
- TLS handshake time if used;
- first-byte latency;
- total response time;
- retry behavior.

### 38.2 MQTT

Record:

- broker connect time;
- keepalive behavior;
- reconnect time;
- QoS behavior;
- duplicate handling;
- offline queue limits.

### 38.3 Local web server

Record:

- concurrent clients;
- response latency during TFT updates;
- memory use;
- behavior during Wi-Fi reconnect;
- authentication and exposure risk.

### 38.4 OTA

OTA testing belongs to firmware-update documentation, but wireless qualification should record transfer reliability, power integrity and recovery after interruption.

## 39. Bluetooth verification plan

Because the current repository has no Bluetooth-specific sample, begin conservatively.

### 39.1 BLE advertising

Test:

- advertising visibility;
- device name;
- advertising interval;
- RSSI at fixed points;
- connection establishment;
- reconnect behavior;
- coexistence with Wi-Fi.

### 39.2 BLE GATT

Test:

- service discovery;
- read/write characteristics;
- notification loss;
- maximum practical payload;
- long-duration connection;
- peer phone and operating-system differences.

### 39.3 Bluetooth Classic

Only add a Classic profile after defining the real use case and software stack.

Potential tests include:

- serial profile throughput;
- pairing behavior;
- reconnect;
- security mode;
- coexistence with Wi-Fi.

## 40. Low-power behavior

Wireless power-save and sleep modes can change:

- connection latency;
- packet delay;
- current consumption;
- access-point compatibility;
- touch or display responsiveness;
- wake behavior.

Record the exact mode and framework version.

Do not infer low-power performance from average current alone. Measure wake peaks and missed data.

## 41. Startup and boot behavior

Radio initialization occurs after ESP32 boot and application startup.

Potential failures include:

- brownout during association;
- blocking startup interface;
- watchdog reset during a long connection loop;
- corrupted credentials;
- access point unavailable;
- DNS or NTP timeout;
- boot-loop caused by unbounded retry logic.

The user interface should distinguish offline operation from a fatal hardware fault.

## 42. Disconnect handling

A disconnect may be caused by:

- access-point restart;
- weak signal;
- authentication failure;
- channel change;
- DHCP issue;
- power droop;
- stack reset;
- interference;
- application starvation.

Record the reason code where the framework exposes one, but interpret it with the matching SDK documentation.

## 43. Captive portals and restricted networks

A board may obtain an IP address yet lack normal Internet access because of:

- captive portal login;
- client isolation;
- blocked DNS;
- blocked NTP;
- firewall rules;
- enterprise authentication;
- MAC allow lists.

Do not diagnose this as antenna failure without local-network tests.

## 44. Static IP and DHCP

DHCP is convenient but introduces dependency on the access point.

If static addressing is used, verify:

- no address conflict;
- correct subnet;
- correct gateway;
- DNS configuration;
- maintainable deployment process.

A link-layer connection with incorrect IP settings can show strong RSSI while all application traffic fails.

## 45. Antenna-area mechanical inspection

Inspect the module antenna region for:

- solder contamination;
- conductive debris;
- adhesive residue;
- metal tape;
- damaged PCB edge;
- cracked module;
- enclosure contact;
- cable pressure;
- rework wire crossing the antenna.

Do not scrape or modify the antenna pattern.

## 46. Comparative enclosure experiment

Use one board and one fixed access point.

Compare:

```text
A: bare board
B: plastic rear cover only
C: complete plastic enclosure
D: complete enclosure with intended cables
E: enclosure with battery or speaker installed
```

At each state, record:

- median RSSI;
- packet loss;
- TCP throughput;
- connect time;
- regulator temperature;
- resets.

Change one mechanical variable at a time.

## 47. Comparative board-sample experiment

When multiple boards are available:

- use identical firmware;
- use identical orientation;
- use the same access point and channel;
- use the same power supply;
- randomize test order;
- record module and PCB markings;
- retain raw data.

A single weak board may indicate assembly variation, damage or module variance rather than a design-wide limitation.

## 48. Failure diagnosis

### 48.1 No 2.4-GHz network is found

Check:

1. access point really has 2.4 GHz enabled;
2. SSID is present on a compatible channel;
3. framework and scan call succeed;
4. board is not repeatedly resetting;
5. antenna area is unobstructed;
6. another known device can see the network;
7. power rail is stable.

### 48.2 Network is found but authentication fails

Check:

- exact SSID;
- password and character encoding;
- security mode;
- hidden SSID handling;
- enterprise versus personal network;
- access-point allow/deny rules;
- corrupted provisioning data.

Do not print the password during diagnosis.

### 48.3 Association succeeds but no IP is obtained

Check:

- DHCP server;
- address pool exhaustion;
- client isolation;
- VLAN configuration;
- static-IP leftovers;
- gateway and subnet settings.

### 48.4 IP is obtained but NTP fails

Check:

- DNS;
- Internet route;
- UDP filtering;
- captive portal;
- server hostname resolution;
- clock timeout handling.

### 48.5 RSSI is strong but throughput is poor

Possible causes:

- interference;
- retransmissions;
- access-point congestion;
- power-save latency;
- Bluetooth coexistence;
- CPU starvation;
- small application buffers;
- TCP window behavior;
- server limitation;
- SD or TFT blocking.

### 48.6 RSSI drops when installed in enclosure

Inspect:

- metal fasteners;
- speaker placement;
- battery position;
- cable routing;
- conductive coating;
- wall thickness and material;
- antenna clearance.

### 48.7 Board resets during Wi-Fi connection

Capture:

- 3.3-V rail;
- USB VBUS;
- current;
- EN;
- reset reason;
- regulator temperature.

Reduce peripheral loads and repeat.

### 48.8 Connection is unstable only with SD or audio active

Investigate:

- power transients;
- scheduler blocking;
- interrupt load;
- heap pressure;
- cable routing near antenna;
- regulator thermal rise.

### 48.9 Bluetooth works but Wi-Fi throughput falls

Treat as coexistence behavior until measured otherwise. Record both workloads and test reduced duty cycle.

### 48.10 Wireless range changes when USB cable moves

Fix the cable geometry and compare direct-host versus independent power. The cable may alter both RF pattern and power quality.

## 49. Test matrix

| Test | Board state | Enclosure | Wireless mode | Other load | Primary result |
|---|---|---|---|---|---|
| network scan | bare | open | Wi-Fi scan | TFT static | RSSI and channel map |
| station connect | bare | open | STA | TFT static | connect time and IP |
| NTP clock | bare | open | STA | TFT active | synchronization reliability |
| orientation sweep | fixed location | open | STA | minimal | RSSI, loss, throughput |
| enclosure comparison | fixed location | staged | STA | intended | RF degradation |
| TCP upload/download | intended | closed | STA | minimal | sustained throughput |
| UDP loss test | intended | closed | STA | minimal | loss and jitter |
| SD logging stress | intended | closed | STA | SD writes | resets and backlog |
| audio stress | intended | closed | STA | speaker active | RF and power effects |
| BLE advertising | intended | closed | BLE | Wi-Fi off | visibility and range |
| coexistence | intended | closed | Wi-Fi + BLE | TFT active | mutual degradation |
| AP reboot | intended | closed | STA | normal app | reconnect behavior |
| overnight idle | intended | closed | STA | normal app | long-term stability |

## 50. Measurement record template

```text
Board identification:
PCB marking/revision:
ESP-WROOM marking:
Firmware commit:
Framework and SDK:
Date:
Operator:

Power source:
USB cable/hub:
Enclosure state:
Board orientation:
Nearby metal/cables:
Speaker/battery installed:
Ambient temperature:

Access point:
AP firmware:
AP channel:
Channel width:
Security mode:
AP location/height:
Distance and obstacles:

Wi-Fi mode:
Connect time:
SSID/BSSID, redacted:
IP acquisition time:
RSSI median/min/max:
TCP up/down:
UDP loss/jitter:
Latency percentiles:
Disconnect count/reasons:
Reconnect time:

Bluetooth mode:
Advertising/connection settings:
Peer device:
Range:
Throughput/notification loss:
Coexistence result:

3.3-V minimum during radio activity:
5-V minimum:
Regulator temperatures:
Reset reasons:

Open questions:
Raw logs:
Conclusion:
```

## 51. Evidence levels for future updates

Use explicit labels:

- **Photo-confirmed** — module marking or physical placement visible;
- **Module-documented** — capability stated for ESP-WROOM-32 family;
- **Repository-configured** — behavior present in current source code;
- **Software-observed** — reproduced on this sample;
- **Power-verified** — rail waveform measured during radio activity;
- **RF-measured** — RSSI, throughput, loss or range recorded under declared conditions;
- **Enclosure-qualified** — tested in the final mechanical assembly;
- **Coexistence-qualified** — Wi-Fi and Bluetooth tested concurrently;
- **Stress-qualified** — passed a declared long-duration matrix;
- **Open** — unresolved.

A successful NTP example must not be promoted directly to “wireless system qualified.”

## 52. Claims that must not be made yet

Until measurements are recorded, do not claim:

- a specific maximum indoor or outdoor range;
- a guaranteed RSSI at any distance;
- a maximum Wi-Fi throughput;
- stable operation in a metal enclosure;
- immunity to display, SD or audio activity;
- a measured antenna radiation pattern;
- antenna efficiency or gain for the installed board;
- an external-antenna option;
- certified operation after RF modification;
- Bluetooth range or throughput;
- Wi-Fi/Bluetooth coexistence performance;
- a qualified regional channel configuration;
- a guaranteed security-mode matrix;
- regulator headroom during radio bursts;
- identical performance across all ESP32-2432S028 revisions.

## 53. Completion criteria

The wireless subsystem can be considered sample-qualified when:

1. ESP-WROOM-32 marking and antenna location are archived;
2. final enclosure and cable geometry are documented;
3. antenna clearance is measured and photographed;
4. 2.4-GHz station connection is repeatable;
5. DHCP, DNS and NTP stages are logged separately;
6. orientation sweep is complete;
7. final-location range points are tested;
8. TCP and UDP tests are complete in both directions;
9. latency percentiles and packet loss are recorded;
10. power rails are captured during radio bursts;
11. regulator temperatures are measured;
12. TFT, SD, audio and external-load stress tests pass;
13. access-point restart and reconnect recovery pass;
14. overnight operation passes;
15. Bluetooth modes required by the project are tested;
16. coexistence is tested if Wi-Fi and Bluetooth are concurrent;
17. credential and transport-security handling are reviewed;
18. regional settings and deployment assumptions are recorded;
19. raw logs and firmware commit are linked;
20. unresolved claims remain explicitly marked open.

## 54. Open questions

- What exact ESP32 silicon and module Flash revision is fitted?
- What is the final antenna clearance in the intended enclosure?
- How much does the TFT assembly distort the pattern?
- Does the USB cable improve or reduce performance in the installed orientation?
- Which regulator supplies the radio domain?
- What is the minimum 3.3-V rail during transmit bursts?
- What are the regulator temperatures during sustained transfer?
- What is the highest repeatable TCP throughput?
- What packet-loss limit is met at the farthest required point?
- How quickly does the application reconnect after AP restart?
- What disconnect reasons occur during long tests?
- Which Wi-Fi security modes are required and verified?
- Is SoftAP provisioning required?
- Which BLE or Bluetooth Classic use case is required?
- What coexistence degradation occurs under the real workload?
- Does the speaker, battery or metal mounting hardware detune the antenna?
- Are multiple board samples statistically consistent?

## 55. Summary

The photographed board uses a classic **ESP-WROOM-32** module with an integrated 2.4-GHz PCB antenna. The repository currently demonstrates Wi-Fi station operation and NTP time synchronization, with a bounded 20-second initial connection attempt.

The antenna end must remain clear of metal, batteries, speaker hardware, cable bundles and enclosure walls where possible. Final performance cannot be inferred from module capability alone; it must be measured with the real enclosure, power source, display, SD card, speaker, cables and application workload.

Wireless qualification should report connection stages, RSSI statistics, packet loss, latency percentiles, throughput, reconnect behavior, power-rail minima, regulator temperature and Wi-Fi/Bluetooth coexistence rather than relying on a single successful connection.