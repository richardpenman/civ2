* The list number itself is our arbitrary number, i.e. duplij-244
* The identifier in parenthesis, such as (L1), indicates which item it is from which list:
  * L means civfanatic duplij's Legacy [list](https://forums.civfanatics.com/threads/civ2-mge-several-fixes-and-additions-with-support-for-music-tracks.688546/)
  * N means civfanatic duplij's New [list](https://forums.civfanatics.com/threads/sid-meiers-civilization-ii-mge-5-4-0f-advanced-mod-1-24-eng-rus.702468/)
  * The number following L or N indicates which number in that list

NOTE: If classification has a question mark (i.e., Fix? or Feature? or Mod?) it means this was Claude's estimation, as-yet-unconfirmed

---

1. <details><summary><del>duplij-1 (L1)</del> - Fix? — Click to expand</summary>

   - Status: Resolved; does not affect OC2 by design
   - Description: Applied NOCD patch and Civ2x64EditboxPatcher patch to fix crashes on 64-bit systems

   </details>

2. <details><summary><del>dupe: duplij-2 (L2)</del> - Feature? — Click to expand</summary>

   - Status: Duplicate; track via duplij-244 (SET1)
   - Description: This executive file contains the ability to listen to tracks from the MUSIC folder. Playable format WAV 44100/22050khz 8/16bit stereo/mono. Tracks are checked for a name in the form TRACK\*\*, so it is advisable to start the initial track with 01 with the possibility of increasing to 99 pieces. Sound playback uses the standard game procedure sndPlaySoundA in asynchronous mode. A new track is played when the previous one ends, provided that a new player turn begins or any left mouse click on the map occurs. To play music, additional RAM is required in the size of the WAV file being played. The number of played tracks is specified in the rules.txt file using the @Cosmic parameter. There are currently 25 tracks in the archive. To use the music playback added in the FoxHead 1.21 patch, instead of this one, set the parameter to 0

   </details>

3. <details><summary><del>dupe: duplij-3 (L3)</del> - Mod? — Click to expand</summary>

   - Status: Duplicate; track via duplij-57 (N1)
   - Description: The font used by the game has been replaced with a thin, condensed Arial, the names of cities and some other game elements remain the same as Times. If the system does not have a Arial Narrow font, the game will use the Times font

   </details>

4. <details><summary>duplij-4 (L4) - Feature?: (also see duplij-156 (N100)) — Click to expand</summary>

   - Status:
   - Description: The number of autosaves has been increased from two to five (\*\*_auto, \*\*_auto0, \*\*_auto1, \*\*_auto2, \*\*_auto3)

   </details>

5. <details><summary><del>dupe: duplij-5 (L5)</del> - Fix? — Click to expand</summary>

   - Status: Duplicate; track via duplij-109 (N53)
   - Description: Fixed a bug when deleting water units in one cell (previously, disbanding a water unit in a cell in which another water unit was present led to the removal of both)

   </details>

6. <details><summary><del>dupe: duplij-6 (L6)</del> - Feature? — Click to expand</summary>

   - Status: Duplicate; track via duplij-83 (N27)
   - Description: Unlocked the ability to set the parameter value in the rules.txt file "Tech paradigm (higher # slows research)" above 10. Expanded the ability to set pays support values for different types of governments from 8 to 100

   </details>

7. <details><summary><del>duplij-7 (L7)</del> - Fix?: (also see duplij-58 (N2)) — Click to expand</summary>

   - Status: Resolved; fixed previously in OC2; see https://github.com/richardpenman/civ2/issues/24#issuecomment-6817643124
   - Description: Added variable initialization that corrects constantly bad relations with AI (taken from FoxAHead)

   </details>

8. <details><summary><del>dupe: duplij-8 (L8)</del> - Feature? — Click to expand</summary>

   - Status: Duplicate; track via duplij-200 (N144)
   - Description: The limit of text lines in the LABEL.txt file has been expanded to 900 lines (also on the file civ2map.exe)

   </details>

9. <details><summary>duplij-9 (L9) - Mod?: (also see duplij-77 (N21)) — Click to expand</summary>

   - Status:
   - Description: Now, when a unit of another race enters the city zone of another player with whom there is a peaceful or allied relationship, the worker processing that cell in the city will not be reset

   </details>

10. <details><summary>duplij-10 (L10) - Feature?: (also see duplij-81 (N25)) — Click to expand</summary>

    - Status:
    - Description: Speed up the movement of units when the option is inactive "Fast piece slide"

    </details>

11. <details><summary><del>dupe: duplij-11 (L11)</del> - Mod? — Click to expand</summary>

    - Status: Duplicate; track via duplij-124 (N68)
    - Description: Now the player will be able to see all units of other players in the stack if they are in sight, except for units in the city

    </details>

12. <details><summary><del>dupe: duplij-12 (L12)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-73 (N17)
    - Description: In single-player games, it is now possible to change the government type of AI players through the cheat-editor

    </details>

13. <details><summary><del>dupe: duplij-13 (L13)</del> - Fix? — Click to expand</summary>

    - Status: Duplicate; track via duplij-124 (N68)
    - Description: Fixed the removal of several air units located in the fortress when they are attacked by another air unit. Now, when one unit is defeated, all others will also be deleted

    </details>

14. <details><summary><del>dupe: duplij-14 (L14)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-124 (N68)
    - Description: Added the ability to edit the terrain cell under the city

    </details>

15. <details><summary><del>dupe: duplij-15 (L15)</del> - Fix? — Click to expand</summary>

    - Status: Duplicate; track via duplij-168 (N112)
    - Description: An air base located inside a city area will no longer provide maximum resources to the city. The city will receive only the basic resources of the cell with the airbase. The road will still provide commerce

    </details>

16. <details><summary><del>dupe: duplij-16 (L16)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-73 (N17)
    - Description: Added the ability to lay out riverbeds and install/remove cities through the cheat-editor SHIFT-F8 (to install a city, there must be any unit in the cell to determine the identity of the new city)

    </details>

17. <details><summary><del>dupe: duplij-17 (L17)</del> - Mod? — Click to expand</summary>

    - Status: Duplicate; track via duplij-245 (SET2)
    - Description: Two resources have been unlocked in the grassland, named Village and Settlement. You can set resources in the rules.txt file. Corrected pictures in the TERRAIN1.GIF file. Corrected the Civilopedia help to account for these resources correctly. Added the ability to display new resources using the map editor Civ2Map.exe

    </details>

18. <details><summary><del>dupe: duplij-18 (L18)</del> - Combo? Fix?/Mod? — Click to expand</summary>

    - Status: Duplicate; track via duplij-124 (N68)
    - Description: The defensive bonus that terrain gives to units has been fixed. Flying units will no longer receive the terrain bonus. For water and flying units it will be equal to the base value of 2 (100%). The river gives ground units an additional bonus of +50%

    </details>

19. <details><summary>duplij-19 (L19) - Fix? — Click to expand</summary>

    - Status:
    - Description: The defense bonus and movement bonus on resource tiles are now taken into account regardless of the base terrain in the Rules.txt file

    </details>

20. <details><summary><del>dupe: duplij-20 (L20)</del> - Fix? — Click to expand</summary>

    - Status: Duplicate; track via duplij-251 (SET8)
    - Description: Now, when you click on a cell that is not open by the player, if there is a unit or city of another player there, the map will still be centered. In open map mode, double-clicking on any open cell that was previously not visible will bring up the terrain information panel. In normal map mode, if a city or a troop stands on a previously opened area of the map, but they are not visible to the player, double-clicking on the cell will bring up the landscape information panel

    </details>

21. <details><summary><del>dupe: duplij-21 (L21)</del> - Fix? — Click to expand</summary>

    - Status: Duplicate; track via duplij-149 (N93)
    - Description: The mechanism for moving units using the GOTO command has been fixed at the gluing point where the X coordinate changes to 0 in a round world. Now units will not move in the opposite direction

    </details>

22. <details><summary><del>dupe: duplij-22 (L22)</del> - Mod? — Click to expand</summary>

    - Status: Duplicate; track via duplij-246 (SET3)
    - Description: Added the ability to build a Harbor and an Oil Platform for cities if the city has access to at least one water cell in the coverage area. If there is not a single suitable area for the Super Highway in the city's coverage area (desert, plain, grassland), it will not be possible to build it

    </details>

23. <details><summary><del>dupe: duplij-23 (L23)</del> - Mod? — Click to expand</summary>

    - Status: Duplicate; track via duplij-163 (N107)
    - Description: The check for AI capitalization by players has been changed; if there are few units left in the city, the capitalization mode will be reset

    </details>

24. <details><summary><del>dupe: duplij-24 (L24)</del> - Fix? — Click to expand</summary>

    - Status: Duplicate; track via duplij-150 (N94)
    - Description: Fixed the display of the text string in windows called from the 'Advisers' menu (F1-F6). The word Republic will no longer be displayed when using the Democracy government system

    </details>

25. <details><summary><del>dupe: duplij-25 (L25)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-159 (N103)
    - Description: In the rules.txt file, a parameter has been added to the @Cosmic category that allows you to set the possibility of technology theft by spies when capturing cities or when interacting with them. If prohibited, instead of stealing technology, spies and diplomats of other states will inspect the city during peaceful relations and possibly sabotage during hostile ones

    </details>

26. <details><summary><del>dupe: duplij-26 (L26)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-187 (N131)
    - Description: The music will stop when the diplomacy screen opens, when the window for calling council consuls opens, when the game is exited from the menu and when a video about a completed great project starts playing. Added an additional trigger for calling a new music when pressing any unit order button or just an action. Since the musical procedure used in the game cannot load each new track after the completion of the previous one, triggers for calling a new track upon completion of the previous one have been added (clicking the left mouse button on the map, pressing any order or action button, as well as at the beginning of a new player turn)

    </details>

27. <details><summary><del>dupe: duplij-27 (L27)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-187 (N131)
    - Description: Added when you click on the button located near the zoom buttons (top left) of the map, random change of music track

    </details>

28. <details><summary><del>dupe: duplij-28 (L28)</del> - Mod? — Click to expand</summary>

    - Status: Duplicate; track via duplij-245 (SET2)
    - Description: The resource value of grassland has been changed, now by default it does not provide production in @TERRAIN, however, if there is a mark in the form of a shield on grasslands, the cell will receive an additional one production. If the shield coincides with a resource on a grassland tile (Village or Settlement) it will give them one more production, if not, then the production will be the base one, which is indicated for the Village and Settlement in @TERRAIN

    </details>

29. <details><summary><del>dupe: duplij-29 (L29)</del> - Fix? — Click to expand</summary>

    - Status: Duplicate; track via duplij-252 (SET9)
    - Description: The diplomatic background has been corrected when there is contact with another race that has a government type of communism or anarchy (file mk.dll)

    </details>

30. <details><summary><del>dupe: duplij-30 (L30)</del> - Mod? — Click to expand</summary>

    - Status: Duplicate; track via duplij-252 (SET9)
    - Description: Added alternative terrain and units to the ALT folder, as well as an option using portraits of leaders Stalin and Hitler when playing for the USSR or Germany (may be useful in World War II scenarios). These options are optional, if you want to use them, copy the contents of the folders to the game folder or ignore it

    </details>

31. <details><summary>duplij-31 (L31) - Feature?: (also see duplij-61 (N5)) — Click to expand</summary>

    - Status:
    - Description: Added a message about the completion of construction of any city unit, not just non-military units. Depends on the active option "Show completion of construction of all units"

    </details>

32. <details><summary><del>dupe: duplij-32 (L32)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-247 (SET4)
    - Description: The number of displayed units in city support has been increased from 8 to 18 units (three rows of 6 units each). The number of selectable units from one cell has been increased from 9 to 18 units

    </details>

33. <details><summary><del>dupe: duplij-33 (L33)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-248 (SET5)
    - Description: The maximum size of a city in which all specialists can be managed manually has been increased from 36 to 52. At the same time, the 37th specialist will work together with 21, the 38th in tandem with 22, etc. This had to be done due to limited address space

    </details>

34. <details><summary><del>dupe: duplij-34 (L34)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-159 (N103)
    - Description: A variable has been added to the rules.txt file, which allows you to set a limit after which global warming will begin or disable it completely

    </details>

35. <details><summary><del>dupe: duplij-35 (L35)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-242 (N186)
    - Description: Added a variable to the rules.txt file that allows or prohibits you from building nukes in cities

    </details>

36. <details><summary>duplij-36 (L36) - Feature?: (also see duplij-86 (N30)) — Click to expand</summary>

    - Status:
    - Description: Added the ability to set the minimum distance between an existing and a new city in the form of specifying variables in the rules.txt file of the @Cosmic parameter

    </details>

37. <details><summary>duplij-37 (L37) - Feature?: (also see duplij-173 (N117)) — Click to expand</summary>

    - Status:
    - Description: Added the ability to create very large maps (up to 65535 cells in size) both in the map editor (Civ2Map.exe) and through the new game menu. Two templates for creating maps have been added to the menu when creating a map: very large and huge. Slightly increased the amount of land when choosing to create maps with a lot of land. Although large maps work, the AI on such maps may play worse due to the large number of objects and large distances. This addition may break compatibility with the CivUIA patch, so the file in the folder (Without_very_large_maps) containing all positions except 37 and 41 is saved separately

    </details>

38. <details><summary><del>dupe: duplij-38 (L38)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-175 (N119)
    - Description: Added a variable to the rules.txt file that allows or prohibits destroying units in stacks one at a time outside key points (such as a city, fortress, airbase)

    </details>

39. <details><summary><del>dupe: duplij-39 (L39)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-192 (N136)
    - Description: The minimum value of the "Communism is equivalent of this palace distance" item in the rules.txt file that can be set has been changed from 1 to 0, so that communism can be completely corruption-free

    </details>

40. <details><summary><del>dupe: duplij-40 (L40)</del> - Fix? — Click to expand</summary>

    - Status: Duplicate; track via duplij-192 (N136)
    - Description: The display of the launch file icon in the upper left corner when registering the main window has been corrected. Now loading the first ICON image from the program resources is explicitly set in the program (this applies to the civ2 and civ2map exe files)

    </details>

41. <details><summary><del>dupe: duplij-41 (L41)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-249 (SET6)
    - Description: Added an optional unit window by right-clicking on the map, instead of the previous right-click handler. This processing occurs according to the cell coordinate, if there is a unit in the cell, a procedure with different options is called, while the unit itself does not immediately become active. If there is no unit, it simply moves around the map. Several options have been added to this window in addition to the usual ones, this is sending a unit to the home city (flying units use a game exploit that bypasses the check for flights and fuel if you do not make the last move, so they can be sent without thinking whether they will fly or not), moving a unit in a group to the very top. In the open map (cheat) mode, through this menu you can heal damaged units, remove the assignment of units to the city, create units, send units without a limit on moves and reactivate already departed units by resetting moves (folder Right_mouse_click saved separately)

    </details>

42. <details><summary>duplij-42 (L42) - Feature?: (also see duplij-197 (N141)) — Click to expand</summary>

    - Status:
    - Description: Added support for images that will be displayed in the Civilopedia when viewing buildings, wonders, units and technologies. Images are in the Pictures folder. Supported format is GIF 256 colors

    </details>

43. <details><summary><del>dupe: duplij-43 (L43)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-198 (N142)
    - Description: The window used in Civilopedia has been slightly increased vertically from 640x400 to 640x480. Display parameters for text line names have been adapted to new window sizes to accommodate 12 lines

    </details>

44. <details><summary><del>dupe: duplij-44 (L44)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-198 (N142)
    - Description: Added Improvements.gif file to the Pictures folder from which small images will be taken in the Civilopedia, and sometimes in the game itself during certain events or actions. When using this file, the window in the Civilopedia will be increased to 700x600 in order to better accommodate the new images. If you delete this file, the game will use miniature images only from the ICONS.GIF file with a window size of 640x480, so the player can choose between the old look and the new one

    </details>

45. <details><summary><del>dupe: duplij-45 (L45)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-250 (SET7)
    - Description: The ability to delete your cities in the normal game mode has been activated (by default, this ability became active after activating the cheat mode). Deleting a city occurs when moving the active frame to the target city and pressing the Shift-D keys. If there are units in the city, you will first be prompted to delete them and only then the city

    </details>

46. <details><summary><del>dupe: duplij-46 (L46)</del> - Combo? Fix?/Mod? — Click to expand</summary>

    - Status: Duplicate; track via duplij-227 (N171)
    - Description: A fix has been added to correctly calculate the trading goods Copper and Dyes, and the values for the Uranium product have been changed so that it appears more frequently in the Offer list.

    </details>

47. <details><summary><del>dupe: duplij-47 (L47)</del> - Fix? — Click to expand</summary>

    - Status: Duplicate; track via duplij-226 (N170)
    - Description: Fixed a calculation error with a negative ID when a unit was in a cell with zero coordinates (0,0), which could lead to a crash or graphical distortions that were observed in cheat mode with an open map

    </details>

48. <details><summary><del>dupe: duplij-48 (L48)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-226 (N170)
    - Description: The window and font for text output during Senate hearings have been enlarged

    </details>

49. <details><summary><del>dupe: duplij-49 (L49)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-232 (N176)
    - Description: It is allowed to move the white highlighting frame of the cursor even after all units have completed their turns

    </details>

50. <details><summary>duplij-50 (L50) - Feature?: (also see duplij-158 (N102)) — Click to expand</summary>

    - Status:
    - Description: The 'hidden terrain' option (T key) will no longer hide rivers, but will remove pollution from the map

    </details>

51. <details><summary><del>dupe: duplij-51 (L51)</del> - Fix? — Click to expand</summary>

    - Status: Duplicate; track via duplij-253 (SET10)
    - Description: Fixed a unit image where, when exchanging technologies in Diplomacy and the player received a new technology (if the player has a wonder of the world, Leonardo's Workshop), a window could pop up about upgrading a certain unit and a distorted image of the unit could appear in it

    </details>

52. <details><summary><del>dupe: duplij-52 (L52)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-226 (N170)
    - Description: The city window will now display a red message saying 'Citizens UNHAPPY' instead of 'Citizens' when the number of dissatisfied citizens exceeds the number of satisfied ones when moving resources on city tiles

    </details>

53. <details><summary><del>dupe: duplij-53 (L53)</del> - Mod? — Click to expand</summary>

    - Status: Duplicate; track via duplij-239 (N183)
    - Description: The mk.dll file has updated image resources (229 and 250) responsible for displaying the leaders Louis XIV and Joan of Arc in diplomacy

    </details>

54. <details><summary><del>dupe: duplij-54 (L54)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-240 (N184)
    - Description: The MsgWaitForMultipleObjectsEx flow suspension procedure, which is triggered when there are no messages, has been implemented in all EXE files, which will reduce the CPU load

    </details>

55. <details><summary><del>dupe: duplij-55 (L55)</del> - Fix? — Click to expand</summary>

    - Status: Duplicate; track via duplij-243 (N187)
    - Description: Fixed a rare bug in a calculation related to the Civilizations array that produced a negative value, overwriting data in preceding memory locations. This often reset City Improvements names to null or filled them with garbage data, frequently causing crashes.

    </details>

56. <details><summary>duplij-56 (L56) - Feature? — Click to expand</summary>

    - Status:
    - Description: Added a keypress interceptor in the main map game loop: if the current keyboard layout is standard Russian (code 0419) and Cyrillic characters are entered, their codes are automatically replaced with the corresponding Latin (QWERTY) ones; for all other layouts, the check is ignored (characters remain unchanged)

    </details>

57. <details><summary>duplij-57 (N1) - Combo? Fix?/Mod?: (contains duplij-3 (L3)) — Click to expand</summary>

    - Status:
    - Description: The font used in the game has been changed, mostly replaced by Arial Narrow, thereby reducing the number of places where words and phrases overlapped each other. Times is used in some places, for example, in the names of cities and the numerical display of the city size. Translation was corrected in many places to a more adequate language, many spelling errors were corrected, and several words and phrases were shortened to fit into the window.

    </details>

58. <details><summary><del>dupe: duplij-58 (N2)</del> - Fix? — Click to expand</summary>

    - Status: Duplicate; track via duplij-7 (L7)
    - Description: Initialization of the variable for calculating attitude level has been added, so now attitude level will be calculated correctly in diplomacy, and there will be no excessive AI aggression (taken from FoxAhead).

    </details>

59. <details><summary>duplij-59 (N3) - Mod? — Click to expand</summary>

    - Status:
    - Description: Some values in the diplomacy variables, which may simplify some requests for certain agreements, have been changed.

    </details>

60. <details><summary>duplij-60 (N4) - Mod? — Click to expand</summary>

    - Status:
    - Description: A condition that activates the variable that changes AI attitude towards the player after turn 200 has been changed, lowering attitude level and blocking the conclusion of alliances and even peace agreements if the human player dominated (was Supreme in power rating). There are other conditions for activating the variable, such as a difficulty level higher than Chieftain, the absence of the Great Wall or the UN, and others. The condition has a value instead of 200 -> 250 (3D FA 00 00 00 0F 8E 0A); the value can be selected in any HEX editor (for example, in HexEdit, which is in this distribution, open the desired exe file in it, then F7, a dot on the code, and insert the desired sequence without any brackets, but with spaces, find the chain, change the value, and save; you can convert DEC values into HEX on a calculator in Windows by changing its View to Programmer) by changing FA 00 to any other (for example, F4 01 -> 500). The second value affects the formation of defensive alliances by AI players against a human player when he dominates 3D C8 00 00 00 0F 8E 0C, where C8 00 (after 200 turns) can be changed to a different number.

    </details>

61. <details><summary><del>dupe: duplij-61 (N5)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-31 (L31)
    - Description: Notification of all built units has been added, when they are completed, not just civilian ones. To display notifications, the checkbox in the settings "City Report Options" -> "Show all units built" must be active.

    </details>

62. <details><summary>duplij-62 (N6) - Feature? — Click to expand</summary>

    - Status:
    - Description: The total number of units in the game has been increased from 2048 to 4521 (of which no more than 4421 are military and 100 are non-military units) by moving variables to another area to increase the address space for units (NOT COMPATIBLE with the FoxAhead patch regarding the Units Limit option). The maximum number of all cities in the game has been increased from 256 to 626, also by moving variables to free areas.

    </details>

63. <details><summary>duplij-63 (N7) - Feature? — Click to expand</summary>

    - Status:
    - Description: The limit on the accumulation of over 32,000 gold in the treasury has been removed (taken from FoxAhead).

    </details>

64. <details><summary>duplij-64 (N8) - Feature? — Click to expand</summary>

    - Status:
    - Description: The range of values when creating maps has been increased; now you can create more extensive maps up to X \* Y = 32768 (taken from FoxAhead).

    </details>

65. <details><summary>duplij-65 (N9) - Feature? — Click to expand</summary>

    - Status:
    - Description: The limit on the maximum total population of over 320,000,000 citizens has been removed (taken from FoxAhead).

    </details>

66. <details><summary><del>dupe: duplij-66 (N10)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-247 (SET4)
    - Description: The window, which is often used in the game when selecting units from one square (stack), when gifting units to another civilization, etc., has been increased from a maximum of 9 to 20 units (16 in civ2hc). The Civ2UIA patch changes the approach to the window and adds a scrollbar to it, and also allows you to scroll it with a mouse wheel.

    </details>

67. <details><summary>duplij-67 (N11) - Mod? — Click to expand</summary>

    - Status:
    - Description: The capacity of aircraft carriers, submarines, and other units with the 'Submarine advantages/disadvantages' flag has been adjusted (paragraph 115 allows the player to set the limit themself). In general, little was known about this capacity until the code was looked at. A submarine can only carry missiles; an aircraft carrier can carry missiles and aircraft units.

    </details>

68. <details><summary>duplij-68 (N12) - Mod? — Click to expand</summary>

    - Status:
    - Description: The pikemen flag has been changed. Previously, it only affected units with 1 hit point and ground units with two moves. Now it affects all ground and attacking units with 1 hit point or higher and 2 moves or more, thereby adding a 50% defense bonus to a defender. This flag has its own special feature; most likely, it is an idea of the developers, because a fairly large piece of code is written to take into account changes from the damage. The flag is not calculated from the unit's basic moves, but from moves taking into account damage, i.e., if an attacking unit with two moves, having received damage, began to move one move, then it will be able to ignore this flag. In this game, it is used for the pillbox unit in modern times and for the pikemen in ancient times. The pillbox has no moves, so it can only be accessed after construction is completed, since the turn of the move will not go to it. Moveless units will now immediately go into a fortified state by default. The pillbox can be transported using an HCA (Heavy Cargo Aircraft). If, during the HCA activity, you enter the city and right-click on the pillbox to change the order to wait, then fly to the city with HCA, and thus the pillbox will be on board, then move HCA to the desired square and select the pillbox in the HCA cargo list, the pillbox will be in the desired square.

    </details>

69. <details><summary>duplij-69 (N13) - Mod? — Click to expand</summary>

    - Status:
    - Description: Several times in-game, a message pops up after researching certain technologies about selling all barracks. The sale of barracks is cancelled because they also make units veterans (25% stronger; by default, it was 50%), so there is no point in selling them regardless of the epoch. It is worth noting that the barracks maintenance increases by +1 gold after the research of Gunpowder and another +1 gold after the research of Mobile Warfare.

    </details>

70. <details><summary><del>dupe: duplij-70 (N14)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-247 (SET4)
    - Description: Increased capacity of units displayed, both in Units Present in the city window and in Units Supported window. The Units Supported window will get a scrollbar if you use the Civ2UIA patch.

    </details>

71. <details><summary><del>dupe: duplij-71 (N15)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-248 (SET5)
    - Description: The display of City Resources has been slightly shifted; it is not so much compressed now. The placement of Citizens at the very top of the city window has been slightly expanded. The next specialist in the city (41st citizen) that exceeds a size of 40, provided that all resources used by this city could not have been used before, will now also be active but will work in tandem with the first specialist (25th citizen); the second specialist (26th citizen) will be in tandem with the 18th (42nd citizen), the 3rd with the 19th, and so on, up to 16x32.

    </details>

72. <details><summary>duplij-72 (N16) - Mod? — Click to expand</summary>

    - Status:
    - Description: Several conditions have been corrected for barbarians who appear in the game, so the number of their appearances has been increased from 2 to 4 at the "Roving Bands" level of barbarian activity, to 7 at "Restless Tribes", and to 10 when choosing "Raging Hordes". In the modern epoch, the pool of units has been changed for the barbarians, and their number can sometimes be even greater under certain conditions.

    </details>

73. <details><summary>duplij-73 (N17) - Feature?: (also see duplij-12 (L12), duplij-16 (L16)) — Click to expand</summary>

    - Status:
    - Description: Previously, it was impossible to set up another AI government in the Editor menu; now the condition has been corrected, so you can change it for yourself and the AI. In addition, the editor has been updated with the ability to lay riverbeds on the map, as well as found/delete cities through the same menu. In order to found a city, you need to select the desired square with a unit on it, then press Shift+F8 (after activating the cheat mode) and check the City option. The unit is needed to determine the ownership of the new city. The distance from another city must be at least 2 squares.

    </details>

74. <details><summary>duplij-74 (N18) - Mod? — Click to expand</summary>

    - Status:
    - Description: After playing one of the games, I noticed that the AI with terrible environmental conditions in the cities has absolutely no pollution. After digging into the code, it turned out that there was a condition that completely ignored the pollution function, as well as the function of subtracting the cost of improvement maintenance from the budget. This condition has been changed; the AI will fully take into account improvement maintenance, and pollution as 1/10 of the probability of normal pollution that humans receive (if you set it like a human, the AI will simply pollute all the city squares and will not have time to clean them up). To change this, a parameter is displayed in the Rules.txt file, the @Cosmic tag.

    </details>

75. <details><summary>duplij-75 (N19) - Mod? — Click to expand</summary>

    - Status:
    - Description: The amount of gold that AI sometimes offered for participating in a campaign against a third party has been changed. Previously, there was a strict condition of no more than 500 gold; now there will be some calculation using randomness (from ~5% to 18% of the entire AI treasury). Request to join the war from an ally with a gold offer has also been increased by about two times. In addition, variation has been introduced in other requests or demands for gold. The selection has been increased, and the change in the monetary selection in diplomacy has been reduced. When gifting money, on average, the selection ranges from 2.5% to 25% of the player's entire treasury.

    </details>

76. <details><summary>duplij-76 (N20) - Combo? Fix?/Mod? — Click to expand</summary>

    - Status:
    - Description: A bypass has been made to the function that reset the auto-move order with command G if any other unit was encountered nearby; now this function is bypassed, provided that the meeting occurs with another nation with which the player has an alliance, peace, or cease fire and will enter the function with the reset of the order if there is a war, only contact without agreements, or no relations at all. Also added a limitation on destination point: if the auto-move ends in a place where another unit or city is located, then a one-time notification will occur, the auto-move order will be reset, and the unit will not hammer again and again into the same place as before until it spends all the moves (the limitation may not work with the Civ2UIA patch).

    </details>

77. <details><summary><del>dupe: duplij-77 (N21)</del> - Mod? — Click to expand</summary>

    - Status: Duplicate; track via duplij-9 (L9)
    - Description: A condition under which the violation of the city borders by another player removes the worker from the processing square of the city that was invaded by another player has been changed for all types of agreements except for the alliance. Now, a peace agreement has also been added to the exception.

    </details>

78. <details><summary>duplij-78 (N22) - Mod? — Click to expand</summary>

    - Status:
    - Description: The game has an implementation in which, if there is an alliance between the players, the players can gradually repair units if they end their turn by entering the city of an ally or remain in the territory of a city that has the necessary improvements. When entering a city, there will be no actual entry, but a function will be activated that will repair the unit by a certain percentage. Now the repair will be more thorough and will restore the unit to be stronger, somewhere two or more times, but on the condition that the AI ally in the city has either barracks, a port facility, or an airport, in accordance with the type of unit that needs to be repaired; otherwise, the repair will be minimal. Repairs now also apply to air and sea units and not just to ground ones, as it was originally.

    </details>

79. <details><summary>duplij-79 (N23) - Mod? — Click to expand</summary>

    - Status:
    - Description: Air units can now land in the vicinity of an allied city if they end their turn as explained in paragraph 22 (i.e., with an entry into an allied city); thereby, they will be supplied with fuel as if they had visited their base, and the range will be reset. Air units are also allowed to fall asleep (command S) outside the city (within one square from the city center; range will not increase).

    </details>

80. <details><summary>duplij-80 (N24) - Mod? — Click to expand</summary>

    - Status:
    - Description: Air units, when flying through their city, will now not lose turns as before, except if there are two or fewer moves left when moving to the city. Flying through the city resets the range if there is at least one range less than the full number.

    </details>

81. <details><summary><del>dupe: duplij-81 (N25)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-10 (L10)
    - Description: Accelerated movement of all automated units on the map when the "Fast piece slide" option is NOT active in Game Options, so that this is noticeable during AI moves; you also need to activate the "No pause after enemy moves" and "Show enemy moves" options. The speed can be changed by changing the number 06 in the HEX editor C7 85 3C FC FF FF 06 00 00 00, where a decrease will speed up the movement, and an increase will slow it down. By default, it was 10. To speed up the movement during AI moves, you can press and hold the Shift key.

    </details>

82. <details><summary>duplij-82 (N26) - Mod? — Click to expand</summary>

    - Status:
    - Description: A condition under which any player unit would wake up (cancellation of the S order) if it met another player has been changed. Now this order will only be cancelled if there is any relationship between the players other than alliance, peace, and cease fire.

    </details>

83. <details><summary>duplij-83 (N27) - Combo? Feature?/Mod?: (contains duplij-6 (L6)) — Click to expand</summary>

    - Status:
    - Description: The bonuses received from special resources in this modification have been changed; the attack and defense factors of many units, especially of the Modern Epoch, have been redefined, and several new technologies and units have been added. Interlayer technologies allow you to move a little faster when changing units with the rapid development of technologies. A value has been added for new technologies that will allow the AI to research them too (up to a maximum of 100 technology pieces can be added by default, but the AI will not research new technologies; you need to specify in the code the last two technologies up to which the AI will allocate funds from the budget for research; now these are Energy Transition and Mobile Warfare III, and by default it was Fusion Power and The Laser). To increase the cost of each new technology, you need to adjust the Tech paradigm parameter (higher # slows research) in the Rules.txt file, @Cosmic tag, by increasing the value (was 10 by default). This parameter was previously locked in the code to a maximum value of 10; now it is unlocked and can be increased in a new and current game. A new parameter, which is responsible for the maximum cost of the technology, has been added to the @Cosmic tag list.

    </details>

84. <details><summary>duplij-84 (N28) - Mod? — Click to expand</summary>

    - Status:
    - Description: The difficulty level limitation that displayed the number of AI units in the intelligence and diplomacy report, when an embassy was present, has been shifted. Previously, these values were shown only at the first two easy levels; now they will be at all levels except the last, most difficult one (Deity).

    </details>

85. <details><summary>duplij-85 (N29) - Mod? — Click to expand</summary>

    - Status:
    - Description: In connection with paragraph 18, which added pollution, the condition leading to global warming has been slightly shifted; now it will be triggered at a value of 20, and not 16 as it was originally. The value can be changed in the Rules.txt file, @Cosmic tag.

    </details>

86. <details><summary><del>dupe: duplij-86 (N30)</del> - Feature? — Click to expand</summary>

    - Status: Duplicate; track via duplij-36 (L36)
    - Description: A condition for founding cities has been changed; now, a human for himself and the AI can choose in the settings (Rules.txt file, @Cosmic tag) the minimum distance between an existing and a new city (decimal values below). 08 - as it was originally for a human, a limit of one square from the city centre; 20 - a limit of two squares, except for the vertical and horizontal position - in one square there; 25 - a limit of two squares everywhere (like a city after expansion); 45 - a limit of three squares, except for the vertical and horizontal position - in two squares there; 49 - a limit of three squares everywhere; 77 - a limit of four squares, except for the vertical and horizontal position - in three squares there; 81 - a limit of four squares everywhere. It was 20 for AI and 08 for a human, by default. The player can change the limitation separately from the AI behavior by changing the following code values from 90 39 5D FC to 83 7D FC 2D, where instead of 2D, put any value from the list above by converting it to a hexadecimal value.

    </details>

87. <details><summary>duplij-87 (N31) - Combo? Feature?/Mod? — Click to expand</summary>

    - Status:
    - Description: All sorts of units have been added to the game, up to the maximum allowed by the game. In particular, two units are provided with the ability to transport units, one from the Industrial Revolution Epoch, an Airship, and the second from the Modern Epoch, a Heavy Cargo Aircraft (HCA). Previously, the game allowed only sea units to carry out transportation; a change has been made to the code for air units. So, in order for a unit to be transported, it is necessary to give it the order S (sleep), and in this state, fly on it with a transport unit, or vice versa, with the unit that needs transport, go under the air unit and press S.

    </details>

88. <details><summary>duplij-88 (N32) - Mod? — Click to expand</summary>

    - Status:
    - Description: The ransom received for capturing the barbarian leader has been reworked, and some randomness has been added instead of the same gold amount tied to the selected level of barbarian activity. So, for capturing a leader at maximum barbarian activity (Raging Hordes), you can get from 16 to 592 gold, at the penultimate activity level (Restless Tribes) from 16 to 448 gold, at the first level (Roving Bands) from 16 to 304 gold, and at zero activity level (Villages Only) from 16 to 160 gold. The cost of a city when barbarians attempt to capture it and demand ransom has changed, and the value has been reduced.

    </details>

89. <details><summary>duplij-89 (N33) - Mod? — Click to expand</summary>

    - Status:
    - Description: A Modern Epoch unit, the Converter, which is strictly tied to the ability to transform terrain, has been added. The processing speed is increased by 3 times compared to the settlers and by one and a half times compared to the engineers. So, settlers put 1 into work each turn, engineers 2, and the converter 3. When the required amount reaches the limit (Rules.txt file, @TERRAIN tag -> Time column), the improvement will be ready. This unit has a role of air and ground unit, i.e., as if hovering, but can be transferred via an airport. The unit should not go far from the shore; otherwise, it may be lost with ~15% chance (if the last move is far from the shore, if you make only two moves, you can bypass this limitation). Can found cities on water, can process ocean squares, build underwater mines, aquafarms, and energy receivers. Aquafarm is divided into two stages: the first one is Aquazone, which is built similarly to irrigation, but does not provide food; the second part is a completed aquafarm itself, similar to a farm, that provides +50% food from an ocean square in the presence of the Supermarket. To operate an aquafarm, first you need a Harbor, since the Supermarket increases food by 50% of the normal one. Road in the ocean is an energy receiver and gives +1 energy (Trade), the railway is an improved energy receiver (it will also be called energy receiver) and increases production by 50% if there are more than two shields in the square (usually this is an ocean resource square with a built mine and oil platform). The ability for sea units to destroy ocean improvements has been added. Superhighways' structure has been edited and will not provide energy to the energy receiver, while Solar Plant, on the contrary, will add +1 energy (Trade) to the improved energy receiver after researching the Energy Transition technology. New names also added new images of improvements, which were introduced to TERRAIN1.gif. AI and automation are also trained to process ocean squares, but they should not settle en masse in the ocean; only occasionally near the coast, covering land squares. Similar to grassland, shields have been added to the ocean squares in the form of a fish icon (wave), where there is an icon it is preferable to build an underwater mine, where there is no icon production from the mine will not increase, because a square without an icon gives -1 production (oil platform on ocean square gives one shield everywhere). The locations of shields in the ocean and grassland can be modified by editing the following code: 03 C9 2B CA F7 D9 03 C1 A8 02. You can change the last value (02) as follows: setting it to 00 will place shields in every square, while a higher value will make shields less frequent. You can also modify other values. For example, you can add 03 to subtract 2B, or do the opposite, or simply reset any two-byte command to 90 90, except for the last one. You can adjust the location of ocean resources by modifying the following value: 45 F8 40 0F 84 07. If you change 40 to 00, ocean resources will be distributed evenly throughout the world during creation, rather than being limited to areas near land.

    </details>

90. <details><summary>duplij-90 (N34) - Combo? Fix?/Mod? — Click to expand</summary>

    - Status:
    - Description: The defense bonus for units with the AEGIS flag has been changed, i.e., against air strikes. From the description, it followed that the defense was increased by two times; in fact, it turned out that it was increased by three times; therefore, this flag has been brought into line with the description. Also, the defense of units with this flag against missiles has been reduced; previously, the defense was increased by 5 times, now it will be 4 times higher (except for the AEGIS cruiser, where it remains 5 times against missiles and 3 times against other aircraft). A 50% defense bonus has been added to the battleship when attacked by a missile.

    </details>

91. <details><summary>duplij-91 (N35) - Mod? — Click to expand</summary>

    - Status:
    - Description: The submarine flag that allows it to be invisible has been slightly reworked. Now this flag is also used for the new UAV drone unit (unmanned aerial vehicle), while detection is bypassed rigidly (i.e. to the number of this unit in the @UNITS tag), so this unit will be completely invisible even to submarines and can only be detected if a unit of another nation tries to enter the same square, or by Radar unit. Like a submarine, a UAV can carry up to 10 missiles on board (can be configured in Rules.txt file -> @Cosmic tag). For the invisibility flag to work on other units, and not just sea ones, it was necessary to make changes that now allow submarines to attack targets ashore, like other ships. UAVs can detect hidden units like submarines. AI sees all units and knows about all human invisible ones, and if it does not immediately destroy them in range, it is only because there are other, more priority targets, or there is no necessary arsenal.

    </details>

92. <details><summary>duplij-92 (N36) - Mod? — Click to expand</summary>

    - Status:
    - Description: A Modern Epoch unit, Sputnik, has been added. This unit has a high attack and a long movement range, but it has low hit points and can be easily disabled. The unit can move anywhere and attack any target. This unit ignores any defensive structures of the city, be it city walls or missile defense, as well as the ability of AEGIS units. It can attack up to two times per turn, depending on the number of available moves. This unit is also strictly (i.e., to the number of this unit in the @UNITS tag) tied to a ban on its attack, so the only units that can attack it are air units with a fighter and missile flag. The same strict ban is made for a Helicopter (except it can also be attacked using an air defense system (ADS)). Sputnik has the role of air superiority, i.e., the 28 moves that it has will be taken into account when transferring units between cities with airports. If enemy Sputnik is within 28 moves of any of the transfer cities, there will be a risk of transport destruction. Only the Apollo Program wonder gives veteran status to a sputnik when it is built.

    </details>

93. <details><summary>duplij-93 (N37) - Mod? — Click to expand</summary>

    - Status:
    - Description: The AEGIS cruiser has been given the ability to carry up to 10 missiles, and it has also been given the ability to attack air targets. The condition for AI missile usage has been changed, namely, the calculation begins by comparing the quadruple cost of the missile with the cost of the target it must hit. If the cost of the missile is less than the cost of the target for the AI, the target is appropriate, in addition to this, other conditions increase the cost of the target, such as: the presence of several units in the fortress, several units of the same type in one square, presence of a city with three units or less, an airbase with several units, several ground attack units in one square or transport, several missiles of any type on carrier.

    </details>

94. <details><summary>duplij-94 (N38) - Feature? — Click to expand</summary>

    - Status:
    - Description: A time modifier for many information windows has been added, that will automatically close the window after 5, 10 and more seconds if the window is not active, i.e. no option is selected in it, or there was simply no mouse/key click (for example, a window about a missile attack or when disbanding a unit), if there was a click, but nothing was pressed, then the window will close after a longer period of time.

    </details>

95. <details><summary>duplij-95 (N39) - Mod? — Click to expand</summary>

    - Status:
    - Description: A small probability of about 5% per turn of changing a government system for the AI has been added, if the AI has Democracy, and there is city disorder lasting more than one turn. From the game description, it follows that in this case, the government should be reset immediately by default. The AI used workarounds, and the government was reset only for the human player. This will bring a certain element of surprise, since the government can be changed to any, except anarchy and despotism, and thus new wars can begin. If the democratic government is reset in this way, then immediately after the reset, a new one will be installed, but if a new one is installed on the next turn, this is then a planned change of the AI system. The more cities in disorder, the more likely it is that the government will fall.

    </details>

96. <details><summary>duplij-96 (N40) - Mod? — Click to expand</summary>

    - Status:
    - Description: The production boost that the AI received for free has been changed. The boost depends on the size of the treasury; the more funds the AI has, the steeper the boost, and the boost in the city is always added regardless of whether the city is in disorder or not, whether there is production there or it is completely exhausted and already at zero. Since all players and AI can accumulate over 32,000 gold, the AI can gradually accumulate so much gold that any unit can be built in one turn. Due to the weakness of the AI, it seems that the main help it gets is given to it by concessions in construction.

    </details>

97. <details><summary>duplij-97 (N41) - Mod? — Click to expand</summary>

    - Status:
    - Description: The ability for your units to be together in a square with an ally outside of cities has been added. To do this, you need to move your unit manually. If you leave your unit together with an ally on the same square, then only one unit will be visible, either yours or your ally's. In this case, you probably won't be able to choose yours if the ally's unit is displayed first; you'll have to wait until it's their turn to move. This mechanism can help overcome a barrier of allied units. Go To move (G) will still bypass all units of other players (changed in paragraph 93). Also, such a scam can provoke an AI attack if your unit is with an ally who is at war, so it is better to use such a maneuver only for transit.

    </details>

98. <details><summary>duplij-98 (N42) - Mod? — Click to expand</summary>

    - Status:
    - Description: New Radar unit and Partisans have a hard-coded +50% bonus in defense against three-move ground units, i.e., tanks. The calculation is based on base moves and does not take into account the reduction in moves from damage. Radar also gets a hard-coded 50% bonus when attacked by three-move ground units. Air Defence System (ADS) (hard-coded) can attack all air units except Sputnik. The game does not seem to check whether air units have any range left at the end of a turn, so both the player and the AI may have situations where missiles or fighters hang outside the base and are active again next turn. This happens to the AI if, for some reason, it cannot attack the target, but has already aimed and flown up to it. The player can also, for example, fly a missile to the right place and end the turn with a Ctrl+N key combination, then in the next turn, the missile will have a full set of moves and will be able to fly further; this is a game exploit. If the unit makes all moves and runs out of fuel, then both the player and the AI will lose a unit; the AI has no way to bypass the loss of a unit that is without fuel (fixed in later versions).

    </details>

99. <details><summary>duplij-99 (N43) - Mod? — Click to expand</summary>

    - Status:
    - Description: A 5% chance that a nuclear missile will penetrate the SDI Defense and strike has been added. If the outcome is unsuccessful, the missile will be destroyed. Only a human can use this. You can change the chance here E5 16 00 B9 14. Instead of 14, put 01 (always overcoming the SDI Defense) or a higher value, thereby reducing the chance of successful penetration.

    </details>

100. <details><summary>duplij-100 (N44) - Fix? — Click to expand</summary>

     - Status:
     - Description: The message about the Casualty Timeline has been fixed. Now, if there are losses, the window will appear at the beginning of your turn, and not at the end as it was before, then it could not have been closed before the beginning of your turn, and it would have interfered with the overview of the AI's moves.

     </details>

101. <details><summary><del>dupe: duplij-101 (N45)</del> - Feature? — Click to expand</summary>

     - Status: Duplicate; track via duplij-249 (SET6)
     - Description: An optional window for a unit by right-clicking on the map has been added instead of the previous right-click handler. This processing occurs based on the square coordinate. If there is a unit in the square, a procedure with different options is called, while the unit itself does not immediately become active. If there is no unit, it simply moves the cursor around the map. Return the previous right-click processing here E8 78 26 07 00 replace with E8 16 09 FF FF. Several options have been added to this window in addition to the usual ones, these are 'Send Unit to the Home City' (it is better not to send air units unless you activate option from paragraph 46, since there is no check for range and the unit can be lost), 'Conduct Training' - training will take place, which will allow NON-veteran to receive this status at some random rate (with each turn, the success probability increases), but in any case, after clicking, the unit will lose all moves, if the training is successful, a corresponding message will pop up. When the whole map is revealed in cheat mode, through this menu, you can heal damaged units, remove units' home cities, create units, send units without limitation on moves, and reactivate already departed units by resetting moves. To remove the restrictions in this mode on unlimited moves for AI, you need to change 85 C3 74 5B, where 5B is changed to 48. To view the map as before, you can still press the 'v' key.

     </details>

102. <details><summary>duplij-102 (N46) - Feature? — Click to expand</summary>

     - Status:
     - Description: A mechanism for air units only, that were using a game exploit, has been added (the game does not check the remaining number of range at the end of the turn if the unit is outside the airbase), which allows you to send units with command G without looking at the number of range. If the unit has the range number that allows movement, it will make the full number of moves. If the range number approaches the limit, the unit will end the turn with one move less and thus will not be destroyed. This mechanism is for humans only; AI bypasses it.

     </details>

103. <details><summary>duplij-103 (N47) - Mod? — Click to expand</summary>

     - Status:
     - Description: A mechanism for calculating the number of ranges for air units has been updated. Now, at the beginning of each turn, all units are fully checked for remaining range. If there are more than or equal to the number allowed, the unit will be destroyed. Accordingly, the option from paragraph 46 will no longer work as intended. A change has also been made to make the option in paragraph 23 work with the new mechanism. All air units can remain within one square of the city without increasing their range; in fact, this radius is an extension of the city, except that range will not be reset. To do this, you still need to fly into the city itself and replenish fuel supplies. An important condition is that, to stop within the city boundaries, you cannot press the spacebar to end a turn for a unit that has no range left, and this should not be the last move of a unit with the last range, otherwise it will be lost. You must either press S or W, and after all other units have ended their turn, the remaining ones must end their turn with the Ctrl+N key combination. This mechanism also works with Helicopter-type units, which can stay outside the base for a long time but lose several hit points every turn. If hit points reach a critical decrease and are equal to the points of destruction or one less, then such a unit will also be lost on the next turn, unless it is in a place that meets the conditions of non-destruction (city or near a city (own or ally), airbase, carrier (for missiles, also an AEGIS cruiser, submarine or UAV)). You can return the operation of paragraph 46, but on the new mechanism for this, you need to change the condition 3B CF 7C 47 to 3B CF EB 47, which will eliminate the removal of air units outside the city and without the remaining range on the new turn, but will leave the addition of completed range.

     </details>

104. <details><summary>duplij-104 (N48) - Mod? — Click to expand</summary>

     - Status:
     - Description: Paratroopers can now jump onto a water city, onto sea units, and onto a section with allied units. When jumping onto sea units, the jump cursor icon will still be crossed out, but the jump will be available. The jump range is 10 squares. You can change the parameters in the Rules.txt file, @Cosmic tag -> Max paradrop range.

     </details>

105. <details><summary><del>dupe: duplij-105 (N49)</del> - Mod? — Click to expand</summary>

     - Status: Duplicate; track via duplij-246 (SET3)
     - Description: Harbor and Offshore Platform can now be built in a city if it has at least one ocean square in its radius, and vice versa - Superhighways can no longer be built if the city does not have a single ground square suitable for a highway in its radius (desert, plains, grassland, and tundra are suitable).

     </details>

106. <details><summary>duplij-106 (N50) - Mod? — Click to expand</summary>

     - Status:
     - Description: Roads will now give +1 gold to terrain not only in desert, grassland, or plains, but also in forest, hills, mountains, tundra, jungle, swamp, and glacier. This applies to all government types, except Anarchy and Despotism, and when Anarchy or Despotism is used, 1 Trade will be subtracted from these squares. On ocean squares, on the contrary, there will be 1 less Trade by default (the option from paragraph 77 now removes 2 Trade from the ocean). Superhighways for these places (forest, hills, mountains, jungle, swamp, and glacier) will NOT add additional gold.

     </details>

107. <details><summary>duplij-107 (N51) - Combo? Fix?/Mod? — Click to expand</summary>

     - Status:
     - Description: A mechanism for calculating pollution has been slightly changed, now the construction of the Recycling Center, Hydro Plant, Nuclear Plant, Hoover Dam and Solar Plant will increase the calculation variable (divider) reducing pollution, rather than setting an exact value in it, also the Solar Plant now levels out pollution not by 100%, thus the relevance of building the Recycling Center after the construction of the Solar Plant remains. Hydro Plant or Power Plant reduces pollution by increasing the divisor by two, the Hoover Dam by three, the Recycling Center adds another four, and the Solar Plant increases the resulting value by six times. A large contribution to pollution (divisible) is made by the Factory, Manufacturing Plant, Power Plant, Hydro Plant, Nuclear Plant, Solar Plant (as manufacturing enterprises, the more production from squares, the more noticeable the effect), a small contribution is made by any improvement built in the city, of which Airport and Offshore Platform act on the divisor variable, reducing it (this will be especially noticeable if the city is small, but already has these improvements), and each new citizen makes his minimal contribution to pollution. Pollution starts to be calculated little by little after researching the Railroad, next strong polluter is the research of Industrialization, then Automobile, and finally Plastics. On average, 24 is subtracted from the calculated pollution so that small cities are not immediately covered in emissions (it was 20, but since there is more food in the game, the city can easily grow higher). If the city becomes an industrial center with production above 100, it may not cope with all the pollution, for such a center, it is better to keep the city size around 20-30 or dismantle some of the improvements. Solar Plant is corrected, now it gives +25% to production (in the original game, from the description it follows that the Solar Plant should give +50% to production, which it does not, but perhaps it should not be a separate improvement, but should be in a combination of Power Plant, Hydro Plant, Nuclear Plant, i.e. you choose to build one thing, but in the game this is not done either on purpose or they did not take this into account). The presence of pollution in a square will now significantly reduce the resource received (it was minus 1 resource; now it is minus 2 of any resource from the square).

     </details>

108. <details><summary>duplij-108 (N52) - Mod? — Click to expand</summary>

     - Status:
     - Description: AI and automation are now trained to build Mass Transit and Solar Plant improvements, since previously the AI bypassed pollution, then Mass Transit was essentially not needed. As a result, the code does not indicate when it needs to build this improvement, similar to the Solar Plant. To build these improvements, the city size must exceed the second growth limit (currently, this is 15). Calculation of the need to build a Recycling Center has also been slightly changed. As soon as pollution appears, the priority of construction will increase. The general connection is that the larger the city size and the more pollution, the higher the priority of construction (however, if the AI is at war, then it may take a very long time for civilian improvements to get in the queue). As for the mechanism of pollution emission in the city, it has also been slightly changed, now when researching most of the technologies and in the presence of a Solar Plant, if pollution is still in the city after this, pollution emission will happen 25% less often (at the difficulty level of Chieftain and Warlord, this mechanism is bypassed, but appearance of pollution there is already reduced) the overall comparison has also been slightly reduced, now the city's pollution will not be multiplied by two for comparison with the calculated number and thus emissions will occur less often, especially if the pollution is small.

     </details>

109. <details><summary>duplij-109 (N53) - Fix?: (contains duplij-5 (L5)) — Click to expand</summary>

     - Status: duplij-5 (L5): fixed on OC2 main; see https://github.com/richardpenman/civ2/issues/24#issuecomment-6817643124; still unresolved: disbanding a transport also disbands its passengers, including HCA air transport
     - Description: A bug that caused the entire stack of sea units to be deleted instead of just one has been fixed. This happened when disbanding sea units if there were more than one in one square. Now each ship will be deleted separately. When disbanding a Transport with units on board, all passenger units will also be disbanded. This also applies to air transport (Heavy Cargo Aircraft (HCA)).

     </details>

110. <details><summary><del>dupe: duplij-110 (N54)</del> - Mod? — Click to expand</summary>

     - Status: Duplicate; track via duplij-245 (SET2)
     - Description: Two resources on the Grassland square type have been unlocked, named Cattle and Garden, added corresponding graphic images to the TERRAIN1.GIF file, and a Description in Civilopedia. If the grassland resource square does NOT match the shield (bush) icon, then there will be one less shield production resource in the square.

     </details>

111. <details><summary>duplij-111 (N55) - Feature? — Click to expand</summary>

     - Status:
     - Description: Changes in terms of creating large maps, increased treasury, unlocking resources in grassland, and Labels.txt size have also been added to the Civ2Map.exe file.

     </details>

112. <details><summary>duplij-112 (N56) - Mod? — Click to expand</summary>

     - Status:
     - Description: Many works and transformations carried out with the terrain have been redefined, as well as the time spent on this. In more detail, what can be transformed with examples is in the Rules_full.txt file, @TERRAIN tag.

     </details>

113. <details><summary>duplij-113 (N57) - Combo? Fix?/Mod? — Click to expand</summary>

     - Status:
     - Description: A certain condition (what value 20 is responsible for is not yet clear), which led to the reset of the peace treaty with the AI player after some time, has been changed. The player did not notice this because, in diplomacy on his side, there was no reset, and in diplomacy, it was indicated that there was an agreement, but if you go for the AI and look at its status, it turned out that instead of peace, there was already just contact, so periodically, suddenly, the AI attacked the player. Now this condition is also tied to the vendetta; if there is such a flag, then the mechanism will be active, and then the variable responsible for the reputation will be connected. If the player who can be attacked has a reputation lower than Excellent, there is a probability of breaking the agreement; the lower the reputation, the greater the probability. I also tied in a variable responsible for power. If the difference is large, for example, the attacking side is Strong, and the one that can be attacked is Supreme, the mechanism will not work, but if the attacking side is Mighty, then it will work.

     </details>

114. <details><summary>duplij-114 (N58) - Mod? — Click to expand</summary>

     - Status:
     - Description: Two verification criteria, by which the AI would filter out only ground units from the list of possible units for construction, have been added. Previously, the AI compared the role of units, their type (or domain), number of moves, defense, and attack strength. If there were matches in these parameters, and in the last three, the next unit was better than the previous one, the AI crossed out the previous unit from the list for construction. Now, for comparison, two criteria have been added: hit points and firepower. This was done so that the AI does not cross out MLRS units, since flags are not compared, and MLRS can penetrate city walls (this flag also penetrates a fortress or fort). In a water city, the AI has been disabled from building engineers to avoid problems with processing ocean squares.

     </details>

115. <details><summary>duplij-115 (N59) - Mod? — Click to expand</summary>

     - Status:
     - Description: The time after which the war status between two AI players was automatically reset has been changed; previously, it was on the 16th turn, now on the 31st. This also affected a human from the AI side, after declaring war it was impossible to communicate with AI for exactly 15 turns, now a certain variable has been introduced that allows you to contact AI several turns after the war (every three turns), and between AI players during the war, on the contrary, the added variable reduces the connection, thus, there should be less frequent situations when after declaring war an AI ally asked for help, and on the next or the same turn he made peace, leaving the human to sort things out on his own.

     </details>

116. <details><summary>duplij-116 (N60) - Mod? — Click to expand</summary>

     - Status:
     - Description: All units except engineers and those with amphibious ability are prohibited from moving from a water city to land and vice versa if the city is near the shore. Amphibious units can attack ground targets on land from transport or from a water city adjacent to land, and vice versa, can attack an adjacent water city from land. AI is also trained to attack a water city with an amphibious ability.

     </details>

117. <details><summary>duplij-117 (N61) - Combo? Fix?/Mod? — Click to expand</summary>

     - Status:
     - Description: The cost of bribing a city with a spy has increased. Cost takes into account the distance from the capital, the size of the city, and the number of improvements built in it, and also checks for the presence of the Courthouse; if there is an improvement, the bribery cost increases by one and a half/two times. The government also contributes: if a bribed player has Communism, then a fixed value is used that determines the distance from the capital; if it is below a certain limit (10), then bribing under Communism will end up being more expensive. The final increase in the bribe turned out to be about 2-3 times. It is worth noting that the main parameter determining the cost of a bribe depends on the treasury of the bribed player; the more gold he has, the more expensive the bribe. Since the limit on the accumulation of gold in the treasury of players over 32,000 has now been removed, bribing can become quite expensive. In addition, I fixed the point that slightly changed the cost depending on the side from which the spy approached the city, if it was closer to the capital of the bribed player, the cost was higher, i.e. from the square where the spy was, it turned out that the distance check is not from the city, but from the unit that causes the action. As a result, I redid it so that the calculation was from the city. A similar bribery system is also for units, where the cost of a unit is multiplied by its distance from the capital, and further, if it is a settler, it is doubled. I almost did not touch this function, except for editing the minimum cost and the item related to communism. For some reason, in the unit function, it was checked not with the bribed side, but with the side of the player who wanted to bribe. I redid it in the same way as in the city function.

     </details>

118. <details><summary>duplij-118 (N62) - Mod? — Click to expand</summary>

     - Status:
     - Description: A function has been unlocked for AI that it previously bypassed, in which the reputation from declaring war is calculated and there is a check for violations of the city borders with a requirement to remove units (does not apply to settlers, diplomat and trade units (caravan and freight), i.e. role 5,6,7), now in such situations AI will remove units from another AI, but there is a small random probability of ~2% that one of AI will refuse to remove units, then a war will break out between them. If this happens, a corresponding message will appear. A new DWAR dialogue has been added to the Script.txt file, and a DHELP dialogue has also been added there when an AI ally starts a war and asks for help. Previously, there were no differences between a defensive war and an attacking one; there was one dialogue, but now it will differ depending on the role of the allied AI.

     </details>

119. <details><summary>duplij-119 (N63) - Mod? — Click to expand</summary>

     - Status:
     - Description: A check for all ground units for being on an ocean square has been added. If a unit is on the ocean square outside of transport or a city, it will drown on the next turn and will be lost. Also, a check for ground units with zero moves on being in a city, fortress, or airbase has been added; if a unit is outside of these places, it will be lost (this check is ignored for a human player). These checks are made to remove lost units, which the AI periodically somehow transfers to different places and leaves, so sometimes units hang out on the ocean square, and a pillbox with zero moves is dropped in some wilderness and remains useless there. For mercenary/barbarian units, transport capacity has been doubled so that barbarian sea raids do not lose units due to a lack of transport capacity when checking.

     </details>

120. <details><summary>duplij-120 (N64) - Mod? — Click to expand</summary>

     - Status:
     - Description: The procedure responsible for how barbarians will act in the game has been slightly changed. There is a separate behavior procedure for them, which does not affect the behavior of other players (the rest of the players have their own large behavior procedure). Mercenaries have been given amphibious ability to attack water cities from transport, and air units, like sea units, can now move across water and attack enemy cities/units. After research of Industrialization, barbarians or mercenaries will be able to appear in ocean squares on transport (a kind of analogue to the Renaissance, and appearing on frigates). After researching most technologies, when mercenaries appear in the ocean, most of their units will consist of amphibious units; if on land, then they will mainly be ordinary units. A small change has been made for ordinary AI players so that they also understand the water city. For a new appearance of mercenaries, a restriction has been set in the ocean no more often than every 7 turns (for barbarian activity levels 0 or 1) and no less than 3 turns (for activity levels 2 and 3). At maximum activity level, mercenaries can spawn closer than 5 squares from the city and in larger numbers. Barbarian attack on AI will not decrease after 150 turns in the barb exe files and after 200 turns for the normal game. Mercenaries that spawn on land and in the late game on Transport use different conditions than those that spawn on Frigates. There are a number of other conditions for their appearance, for example, a randomly selected city is checked, if it is a city of a human player, the check goes further, if the city is of an AI, the check on this turn passes, and there will be no barbarians. Another condition is that up to the 150th turn number of barbarians is reduced for any of the activity levels, and also for activity levels 0 and 1, several checks are bypassed, which, in more advanced times, reduces the number of possible mercenaries. The exe file ending with barb uses the usual player function for barbarian movements, but there is an exception: if the unit does not have a home city, it will act as a barbarian (except for being in the city), in addition, mercenaries in the barb exe file are allowed to build settlers and engineers, and after capturing a city they can begin expansion like a normal player (you can enter a ban here 85 C0 90 90 8B, where instead of 90 90 put 74 39). Since in the barb exe file, mercenaries can build units like other players, the same requirements apply to them, namely the presence of the necessary technology; otherwise, they will not be able to build anything except a 'war' unit. If a human player establishes an embassy with mercenaries, the human player will be able to conclude agreements and transfer technologies to them for their rapid development.

     </details>

121. <details><summary>duplij-121 (N65) - Combo? Fix?/Mod? — Click to expand</summary>

     - Status:
     - Description: A mechanism that switches the cost of technology has been fixed. If the number of technologies of one of the players exceeds that of the other, then the cost of research for the player who has fewer of them is reduced by three times. This applies to both humans and AI. Previously, this mechanism did not work quite correctly. If a player was the first to research a specific technology, it is highlighted in white in the F6 window. Initially, starting cost of a technology for a player is lower than for AI at the Chieftain level by ~142%, at the Warlord level it is lower by ~60%, at the Prince level it is lower by ~17%, at the King level it is higher by ~9%, at the Emperor level it is higher by ~30% and at the Deity level it is higher by ~60%. Also, at a high difficulty level and after the 150th turn, the cost of a new technology is slightly reduced for the AI.

     </details>

122. <details><summary>duplij-122 (N66) - Mod? — Click to expand</summary>

     - Status:
     - Description: Minor changes have been made to the AI government choice. From what I understand, the main parameters are production of all cities, trade, supported units, and corruption losses. Two optimal government types in this regard are Democracy with maximum profit and Fundamentalism with production and units, so all other types usually lose. Therefore, I added small values (the result of the values depends on the number of cities in the game), which are added to one or another government when analyzing and comparing them. So if a player has many wars, the attractiveness of the Republic and Democracy decreases slightly, and increases for other governments. In the presence of alliances with the same government, the attractiveness grows for every government type except for Fundamentalism (for Communism, growth is doubled). With peaceful relations, if someone has Communism, then his attractiveness when chosen by another player is also slightly higher. If the strongest player chooses Communism, his attractiveness is even slightly higher for all others. Also, if there is an alliance between two communist governments, the relationship between them will be slightly higher. The parameter that eliminated waste has been removed in democratic government, so now, although there will be no corruption, a small loss of production may be present.

     </details>

123. <details><summary><del>dupe: duplij-123 (N67)</del> - Combo? Fix?/Mod? — Click to expand</summary>

     - Status: Duplicate; track via duplij-252 (SET9)
     - Description: The background of the communist and anarchist government in diplomacy has been changed; now, next to the portrait of the leader will be a star or a symbol of anarchy, instead of the previous icon of monarchy. Portrait of Frederick of the Germans has been replaced with Hitler (resource images 200, 203, 222 in the mk.dll file have been replaced with the eXeScope program).

     </details>

124. <details><summary>duplij-124 (N68) - Combo? Fix?/Feature?/Mod?: (contains duplij-11 (L11), duplij-13 (L13), duplij-14 (L14), duplij-18 (L18)) — Click to expand</summary>

     - Status: duplij-13 (L13): OC2 code appears not affected, would need more details or a recreate or a .SAV file to dig deeper; see https://github.com/richardpenman/civ2/issues/24#issuecomment-6817643124; still unresolved: the other bundled tweaks (editing improvements on ocean squares, best defender for water cities, SAM/SDI defender firepower, deleting cities in any mode, AI capitalization exit, AI airport transfer rules, AI build priorities, damaged sea unit repair turns, AI carrier builds)
     - Description: Minor tweaks: Terrain defense bonus for air and sea units has been removed (now their terrain defense is always the same as that of a grassland or ocean); now you can change square type under a city in the editor, as well as edit improvements on ocean squares; added a check and selection of the best defender for a water city when attacking it; defenders in a city with SAM Missile Battery and SDI Defense have their firepower increased +1 in defense (except for ships in harbor, there is always 1); cities can now be deleted in any game mode (improved in future versions); fixed mechanism when exiting capitalization for the AI player; several air units in a fortress will now be destroyed en masse when attacked, not one by one; player can now see units in AI stack (except for the city) if they are nearby; AI has been given the same requirements for transferring units through airport as a human, and the AI is also prohibited from transferring units with zero moves; the priorities for building AI units and automation in cities have been slightly redefined; the minimum number of turns for a damaged sea unit has been increased from 2 to 3; in test files, the construction of carrier has been unlocked for the AI, however, as far as I understood, it does not know how to use them (their construction has a very low priority); and some other things.

     </details>

125. <details><summary>duplij-125 (N69) - Mod? — Click to expand</summary>

     - Status:
     - Description: Old saves and scenarios from another game are not compatible with the modification and will not load.

     </details>

126. <details><summary>duplij-126 (N70) - Mod? — Click to expand</summary>

     - Status:
     - Description: A graphic pack of terrain is taken from the Colonization scenario, but mixed with the original, plus some resources are taken from different places. To use a more original terrain, delete the former one and rename TERRAIN\*_orig to TERRAIN\*.

     </details>

127. <details><summary>duplij-127 (N71) - Fix? — Click to expand</summary>

     - Status:
     - Description: A bug with the range in the Ancient Epoch when barbarians appear on a trireme has been fixed.

     </details>

128. <details><summary>duplij-128 (N72) - Mod? — Click to expand</summary>

     - Status:
     - Description: Option from paragraph 40 has been slightly changed; now there is a limit on converting boost gold into shields to a level no higher than 50,000 gold. Another option is to make the entire piece with AI bonuses, the same as a human without boosts. For this, replace 0F 85 CB 08 00 00 with E9 CC 08 00 00 90 (however, this will most likely make the AI completely helpless) or vice versa, make it for a human like the AI, then set 90 90 90 90 90 90.

     </details>

129. <details><summary>duplij-129 (N73) - Feature? — Click to expand</summary>

     - Status:
     - Description: CodecTweakTool_674.exe file has been added to the distribution. In the program, click on the ACM/VMW 86 button and select Indeo 4 codec in the DISABLED category, click Apply/Close, which will allow you to show video clips.

     </details>

130. <details><summary>duplij-130 (N74) - Mod? — Click to expand</summary>

     - Status:
     - Description: Music intro in the game menu has been replaced with a similar melody, Japanese Drum (Oliver Scheffner). To return to the previous one, delete MENULOOP and MENUEND files (Sound folder), and remove the exclamation mark in the !MENULOOP and the !MENUEND files.

     </details>

131. <details><summary>duplij-131 (N75) - Feature? — Click to expand</summary>

     - Status:
     - Description: The ability to delete cities (Shift+D on a selected empty city without units) in network mode (when selecting local network or internet mode) has been improved.

     </details>

132. <details><summary>duplij-132 (N76) - Mod? — Click to expand</summary>

     - Status:
     - Description: Stealth flag has been added to the spy in the Rules.txt file -> @UNITS tag (now they will be difficult to detect).

     </details>

133. <details><summary>duplij-133 (N77) - Mod? — Click to expand</summary>

     - Status:
     - Description: Trade on ocean squares by default is completely removed. Now +1 Trade is given by a Harbor and/or an energy receiver, then if there is at least one Trade, +1 gold is added from the Republic/Democracy government type (or in celebration mode), and then another +1 from the Solar Plant if there is an improved energy receiver. Also added a condition for getting +1 Trade with an improved energy receiver in combination with Solar Plant after researching Energy Transition technology, so this will be given only at the very end of the game.

     </details>

134. <details><summary>duplij-134 (N78) - Mod? — Click to expand</summary>

     - Status:
     - Description: Converter unit will now be available after researching Robotics instead of Energy Transition, so that it can be used before the end of the game. Energy Transition technology is no longer a technology for transforming the terrain, but a technology for converting/obtaining new energy.

     </details>

135. <details><summary>duplij-135 (N79) - Combo? Fix?/Feature? — Click to expand</summary>

     - Status:
     - Description: Spelling errors in Civilopedia descriptions have been fixed; added "Effect of Roads: Trade: +1" description to Civilopedia when viewing terrain (tundra, forest, hills, mountains); unlocked 'Description' button when viewing Civilization Advances in Civilopedia.

     </details>

136. <details><summary>duplij-136 (N80) - Feature? — Click to expand</summary>

     - Status:
     - Description: For each military unit, all unit properties are now specified in detail in Civilopedia description; variables responsible for icons and unit descriptions in Civilopedia have been moved to a free address space, which has allowed all 63 units and their description to be correctly displayed; display of Wonders of the World and City Improvements in Civilopedia when viewing Discoveries will now be a single line (columns have been removed for them), which will allow full name to be displayed without cutting off letters; sentences from Pedia.txt file -> @PEDIACIVFACTS tag have been unlocked, which will now be displayed when viewing Civilization Advances in Civilopedia, for example, when viewing Communism technology, three sentences from this category, that are affected by this technology, will be added.

     </details>

137. <details><summary>duplij-137 (N81) - Mod? — Click to expand</summary>

     - Status:
     - Description: The last free technology has been activated, named Composite Armor; links between the appearance of later units and technologies have been slightly changed.

     </details>

138. <details><summary>duplij-138 (N82) - Fix? — Click to expand</summary>

     - Status:
     - Description: Several errors and inaccuracies have been fixed.

     </details>

139. <details><summary>duplij-139 (N83) - Combo? Fix?/Mod? — Click to expand</summary>

     - Status:
     - Description: Several inaccuracies and bugs, that sometimes caused freezing, have been fixed; airship is now completely untied from the government (as it replaces fanatics) and is available in the game; barbarians/mercenaries should now appear on transports in the Modern Epoch.

     </details>

140. <details><summary>duplij-140 (N84) - Mod? — Click to expand</summary>

     - Status:
     - Description: Barbarian/mercenary diplomat in the barb exe files is given a condition to sometimes perform some action in an enemy city like a normal player; if there are a lot of barbarian units (for example, at the Raging Hordes activity level) and they become very outdated, they will be gradually destroyed after the 200th turn, relevant for barb exe files; barbarians do not spend gold on improvement maintenance if there are any in their cities, their cities do not go in disorder if there are unhappy people (these properties are the same as in the original, except that in the original they cannot choose production in the city and produce only the unit that captured the city, they also cannot change government type and do not spend shields on unit maintenance, since the units they produce do not have a home city).

     </details>

141. <details><summary>duplij-141 (N85) - Mod? — Click to expand</summary>

     - Status:
     - Description: A mechanism for disbanding units in the absence of city support has been changed. Now, if a city is short of support shields, but there are shields invested in any production under construction, the missing shield for support will be doubly subtracted from the construction window at the beginning of a new turn. When shields in the construction window are zeroed out or there are fewer than needed to support the unit on the next turn, the unit will be disbanded. If the unit being disbanded is in the city, part of its production cost will be transferred to the city. AI does not use this mechanism.

     </details>

142. <details><summary>duplij-142 (N86) - Feature? — Click to expand</summary>

     - Status:
     - Description: A parameter has been added to the @Cosmic tag of the Rules.txt file that allows changing the number of units without maintenance for barbarians/mercenaries, i.e., only works for an exe file with the barb prefix. Free units from governments are added to this parameter if possible (monarchy/communism/fundamentalism). Despotism and anarchy give another 1/2 plus of this parameter.

     </details>

143. <details><summary>duplij-143 (N87) - Feature? — Click to expand</summary>

     - Status:
     - Description: The sound framework has been expanded, new variable names have been added to the address field, and new procedures with links to these sounds have been added. Now EXTRA9.WAV sets the sound to the 8th line of units (Airship), EXTRA10.WAV sets the sound to the 7th line of units (Musketeers), EXTRA11.WAV sets the sound to the 18-19th lines of units (Crusaders and Knights), EXTRA12.WAV sets the sound to the 24th line of units (Cannon), EXTRA13.WAV sets the sound to the 40th line of units (Battleship). Seven variables are reserved for other custom sounds (from EXTRA14.WAV to EXTRA20.WAV). To activate them, find code sections below and replace the 3F value with the unit line number from the Rules.txt file, @UNITS tag in hexadecimal. Units are counted from 0 (settlers) to 62 (explorer). Sound is played only for the attack, so units that have zero attack or are non-military do not need a sound. You can set the sound only for those units for which it has not yet been set. Sounds from lines from Air Defense System (ADS) to Heavy Tank are set by sound files from CUSTOM1 to EXTRA8, plus my additions extends the occupied range to EXTRA13, for the remaining units you can set your own sound accordingly, and it will be played instead of the standard one, since these sections are selected by the game earlier than the code with the usual sound. Sound format WAV 8-bit 22kHz stereo or mono in the Sound folder. 83 7D 80 3F 75 16 6A 00 6A 00 6A 01 68 8A 00 00 00 E8 C7 C5 FF FF - EXTRA14.WAV; 83 7D 80 3F 75 16 6A 00 6A 00 6A 01 68 8B 00 00 00 E8 AB C5 FF FF - EXTRA15.WAV; 83 7D 80 3F 75 16 6A 00 6A 00 6A 01 68 8C 00 00 00 E8 8F C5 FF FF - EXTRA16.WAV; 83 7D 80 3F 75 16 6A 00 6A 00 6A 01 68 8D 00 00 00 E8 73 C5 FF FF - EXTRA17.WAV; 83 7D 80 3F 75 16 6A 00 6A 00 6A 01 68 8E 00 00 00 E8 57 C5 FF FF - EXTRA18.WAV; 83 7D 80 3F 75 16 6A 00 6A 00 6A 01 68 8F 00 00 00 E8 3B C5 FF FF - EXTRA19.WAV; 83 7D 80 3F 75 14 6A 00 6A 00 6A 01 68 90 00 00 00 E8 1F C5 FF FF - EXTRA20.WAV.

     </details>

144. <details><summary>duplij-144 (N88) - Feature? — Click to expand</summary>

     - Status:
     - Description: The ability to sell a selected city improvement in all your cities at once has been added. The corresponding line has been added to the City Improvements sell menu.

     </details>

145. <details><summary>duplij-145 (N89) - Feature? — Click to expand</summary>

     - Status:
     - Description: The ability to upgrade obsolete units has been added. To do this, right-click on a unit inside the city or on units displayed in the city 'Units Supported' area and select the appropriate option. 'Upgrade obsolete unit' option upgrades the selected unit, 'Upgrade all obsolete units' option will upgrade all units, not just those in that city, but all obsolete ones. Upgrades are done in stages, so if the army is very obsolete, it must be upgraded several times. The cost of upgrading is determined by comparing the parameters of the old and new units. The difference in production cost, hit points, and number of moves adds more to the cost of upgrading than the difference in attack, defense, or firepower. In general, the upgrade mechanism almost completely copies Leonardo's Workshop mechanism. Added @NOUPGRD, @Lowcash, @UPGCOST, and @UPGRAD tags to the Game.txt file and one entry to Labels.txt.

     </details>

146. <details><summary>duplij-146 (N90) - Mod? — Click to expand</summary>

     - Status:
     - Description: Several unit icons in the UNITS.GIF file have been changed; the appearance and disappearance of several units in the Rules.txt file have been redefined. Unit icons from the previous version have been saved in the UNITS_old.GIF file.

     </details>

147. <details><summary>duplij-147 (N91) - Mod? — Click to expand</summary>

     - Status:
     - Description: A mechanism for calculating taxes or tithe under fundamentalism has been changed. Previously, accruals were made before the calculation of an increase in gold from the MarketPlace, Bank, Stock Exchange, and Tax Men, and thus had a significant impact by adding a lot of gold to the treasury. Now their contribution will be after all calculations, and they will add static gold in the form of a specified collection. It is worth considering that by default in the @Cosmic tag settings, fundamentalism is set to a maximum degree of science of 50%, so if you move the slider for contributions to science above 50%, further investments will be converted into taxes. In addition to this, AI converts all investments in luxury into taxes (into science in the original game), since under fundamentalism, there are no unhappy people, although it does not take into account the fact that if a city does not celebrate, then in totalitarian government, this brings less trade from each city square.

     </details>

148. <details><summary>duplij-148 (N92) - Mod? — Click to expand</summary>

     - Status:
     - Description: To make the game a little more difficult, I added the cost required for unit maintenance. This mechanism is enabled only for humans, but can be enabled for AI as well. Unit maintenance is halved for totalitarian government types, absent under anarchy, and there is an additional +1 gold for all military units' maintenance under democracy. To understand how much is spent on maintaining units, you can open the F5 tab (Trade Advisor), where there is a separate line 'Improvement Maintenance', and just below that, there will be a line 'Unit Maintenance'. At the Deity level difficulty of the game, after 200th turn, +1 gold will be added to each unit's maintenance, regardless of whether it is a military unit or not; a detailed description of setting up cost can be found in the Civilopedia -> Game Concepts -> Unit Maintenance. Text line 'Improvement Maintenance' has been added at the end of the Labels.txt file.

     </details>

149. <details><summary>duplij-149 (N93) - Combo? Fix?/Mod?: (contains duplij-21 (L21)) — Click to expand</summary>

     - Status: duplij-21 (L21): does not affect OC2; see https://github.com/richardpenman/civ2/issues/24#issuecomment-6817643124; still unresolved: passing through allied units on Go To, the 'Long-distance' message and AI move limits, the hex option to disable passing through allies
     - Description: Tried to fix an inaccuracy in the 'Go To' auto-movement, where the transition at the place where the map was glued along the X coordinate was incorrect, and the units went in the opposite direction. This glitch appeared on round maps in which the Y coordinate exceeded the X coordinate, i.e., on all regular maps, which, when created, are designated as 50x80, etc. In fact, the X size in the game then doubles. I did not find a specific reason, which, in my opinion, was strange and incomprehensible; I just removed one of the checks that filtered out the movement of the unit to the new coordinate. After changing it, units began to move more or less normally, crossing the border as they should. However, it is possible that this can cause some problems, for example, an increase in units getting stuck, which already happens when moving, or other glitches, so for now, this is a test version (actually fixed in version 1.15). Overall, the Go To movement is still pretty bad, especially for ships and infantry units that need to respect terrain restrictions. Ships often take longer routes, especially if there are a lot of bends and narrow straits to their final destination, and occasionally get stuck. The ability to pass through allied units when auto-moving via Go To has been added, both for the player and the AI. Sometimes it happens that a message about 'Long-distance' pops up. To activate it, a unit must make 51 moves approximately in one direction or less, if back and forth (about 4 moves without taking into account the contribution of normal moves), then the game seems to understand that the unit probably cannot pass and gets stuck. There is a special limit for AI to make a maximum of 20 (25) moves, so they do not move very far along the railway. If instead of 75 09 you put 90 90 in two places, 92 F5 FF 85 C0 75 09 and here 8C F5 FF 85 C0 75 09, then this will cancel the ability to pass through allies on auto-move for both the player and the AI.

     </details>

150. <details><summary>duplij-150 (N94) - Fix?: (contains duplij-24 (L24)) — Click to expand</summary>

     - Status: duplij-24 (L24): fixed on OC2 main; see https://github.com/richardpenman/civ2/issues/24#issuecomment-6817643124; still unresolved: Advisors window line layout, Democracy shown under Communism, text file fixes
     - Description: Arrangement of text lines in the window called from the 'Advisors' menu has been optimized. An error has been corrected when under the Democracy, the line Republic was displayed in this window, and under Communism - Democracy; several errors in text files have been corrected.

     </details>

151. <details><summary>duplij-151 (N95) - Mod? — Click to expand</summary>

     - Status:
     - Description: All sea units with the ability to transport units have been given priority to first unload at a given target and then sail home or skip a turn for repairs, if a transport is heavily damaged with units on board; parameters of the number of units have been slightly changed (trireme, caravel, galleon, cruiser, transport).

     </details>

152. <details><summary>duplij-152 (N96) - Fix? — Click to expand</summary>

     - Status:
     - Description: A bug, when exiting the game, that displayed an event in the Windows log about the error 0x0004cbc4 has been fixed.

     </details>

153. <details><summary>duplij-153 (N97) - Feature? — Click to expand</summary>

     - Status:
     - Description: File civ2_start_old_scenario.exe has been added, which will allow you to run old scenarios or saves on this modification. (discontinued from version 1.17)

     </details>

154. <details><summary>duplij-154 (N98) - Mod? — Click to expand</summary>

     - Status:
     - Description: Separately added cost for Carrier, Pillbox, HCA units, adding to paragraph 92, since these units have an attack of 0, they did not participate in the calculation. So, Pillbox costs 4 gold, HCA and Carrier 6 gold; an increased number of possible AI moves before the limiter from 20 to 30, so that it can fully use ICBM-type missiles and Sputnik.

     </details>

155. <details><summary><del>dupe: duplij-155 (N99)</del> - Feature? — Click to expand</summary>

     - Status: Duplicate; track via duplij-244 (SET1)
     - Description: The ability to listen to music from the Music folder has been added. The mechanism is implemented on the imported procedure for playing sounds PlaySoundA. Since this is playing sounds, then after playback is complete, the next track does not start itself, so I made a binding for choosing a random track on a new game turn of a human or by left-clicking on a game map. A new track starts only when the previous one ends. You can start a specific track from the Game menu 'Pick Music'. This implementation can play up to 80 tracks in the Music folder. Supported sound format is WAV 44100/22050kHz 8/16bit stereo/mono, called via PlaySoundA procedure with the SND_ASYNC, SND_NODEFAULT, SND_NOSTOP flags. In the Rules.txt file, the @Cosmic tag, an option that specifies how many tracks can be played, has been added. Setting the value to 0 will disable this playback mechanism. Newly added tracks in the form of TRACK\*\* can simply be placed in the Music folder and the @Cosmic tag edited. In addition, you can also add the track name to the Game.txt file, @PICKMUSICGOLD tag, for further manual selection in the menu. Track number TRACK\*\* is equal to the line number.

     </details>

156. <details><summary><del>dupe: duplij-156 (N100)</del> - Feature? — Click to expand</summary>

     - Status: Duplicate; track via duplij-4 (L4)
     - Description: List of autosaves has been expanded from 2 to 5 (Auto, Auto0, Auto1, Auto2, Auto3). The current turn is divided by 5; the remainder of the division is used to select which number will be assigned after 'Auto'.

     </details>

157. <details><summary>duplij-157 (N101) - Fix? — Click to expand</summary>

     - Status:
     - Description: Option in paragraph 99 has been fixed, music being played is linked to the 'Music' flag in the settings menu, unlinked from 'Sound'; other minor edits.

     </details>

158. <details><summary><del>dupe: duplij-158 (N102)</del> - Feature? — Click to expand</summary>

     - Status: Duplicate; track via duplij-50 (L50)
     - Description: In 'Show Hidden Terrain' view mode, the 't' key now displays rivers in areas and removes pollution.

     </details>

159. <details><summary>duplij-159 (N103) - Feature?: (contains duplij-25 (L25), duplij-34 (L34)) — Click to expand</summary>

     - Status:
     - Description: Several variables have been moved to the Rules.txt file under the @Cosmic tag to reduce the number of nested executable files. Now you can set the ability to steal technologies, the global warming limit, as well as the level of AI pollution and city emissions there. In addition, the end-of-game date has been displayed, which can be replaced with your own (after completing the game and choosing to continue playing, a certain flag is set that changes some of the AI's actions, for example, the AI will not build a Spaceship, even if no one has built or launched a Spaceship by the end of the game). The date option in the Civ2UIA patch is now disabled, because it began to cause an error. The level of global warming is calculated approximately as follows: all squares with a pollution icon are counted, cities with a built 'Recycling Center' (previously Solar Plant) are subtracted from them. If the value is negative, global warming is not threatened; if positive, another variable is increased by one each turn, with which the comparison is made. When a critical value is reached, a message appears about the approaching global warming. Somewhere, the transition will occur in two turns, and global warming will be inevitable. Thus, between the appearance of the message about global warming and the process itself, there are about two or three turns to try to do something and prevent it. After the first global warming, another variable increases by four, which adds an increase in range to the calculation. The next global warming will occur when the same limit is reached plus four. Another +4 will be added thirty times to subsequent warmings, and so on up to the maximum limit of 120. If more than one human player is involved in the game, then all pollution is divided by the number of human players. AI, as you can see, was almost not involved here, because its cities emit no pollution by default. With the AI pollution option enabled, emissions will increase.

     </details>

160. <details><summary><del>dupe: duplij-160 (N104)</del> - Fix? — Click to expand</summary>

     - Status: Duplicate; track via duplij-251 (SET8)
     - Description: A display of information about a square by double-clicking the left mouse button has been corrected. Previously, if a square was opened by a player and there was a hidden unit or city of another player on it, then the window with information about the square did not pop up, and thus this indirectly indicated the presence of a unit or city there. Now, information about the square will be shown in any case; the exception is if there is a unit or city of the player himself in the square. In addition, information about the squares will also be displayed in the open map mode, on those squares that are visible at the time of activity of this mode, but were invisible before.

     </details>

161. <details><summary>duplij-161 (N105) - Mod? — Click to expand</summary>

     - Status:
     - Description: A mechanism for ending production in the city has been reworked. Now, if some city improvement is completed and city production has not been changed on the current turn, then on the next turn, a menu will pop up in which you will need to: the first option is to enter the city, where production will automatically switch to capitalization (provided that the player has researched the necessary technology) and select new production. If this is not done and the previous construction is left, then shields will not contribute to production, and shields will be reset every turn. The second option is to sell this improvement without entering the city and build it again. Thus, the ability to accumulate production and quickly complete new wonders of the world that appear with the research of new technologies will disappear. The same mechanism works with Spaceship components when more of them are built than necessary. This mechanism also affects the AI with the choice of the first option of switching to capitalization or zeroing out production; the cost of buying city improvements and wonders of the world has been changed. Now, until the 200th turn, buying wonders of the world is twice as expensive. After the 200th turn, all improvements will be twice as expensive, and wonders and components of the Spaceship will become four times more expensive. If you buy an improvement or wonder from scratch with a shield contribution of less than a quarter of the cost of the entire improvement, then the cost will be doubled again. Anyone who wants to return to how it was in terms of production accumulation can find two pairs of codes with the editor E8 A5 85 F1 FF and E8 0F 85 F1 FF and delete them by replacing them with 90 90 90 90 90. The general process of cost formation is as follows: the column responsible for the cost of production in the Rules.txt file, @IMPROVE tag, is multiplied by the value from @Cosmic responsible for the number of rows in the city's production box. The resulting value is limited to 1000. The shields already spent on construction are subtracted from the resulting value. Then the resulting value is doubled for improvements or quadrupled for wonders. Then there are added coefficients that increase the buying cost of an improvement by another two times, and a wonder by four times, and finally, the accumulation of shields in production is checked; if there are fewer than 40, the cost is doubled again.

     </details>

162. <details><summary>duplij-162 (N106) - Feature? — Click to expand</summary>

     - Status:
     - Description: Description has been added to Civilopedia -> Game Concepts on how cost is calculated, items: Unit Maintenance, Unit Upgrade, and Global Warming calculation; several unnecessary messages popping up when running the game without human participation have been removed.

     </details>

163. <details><summary>duplij-163 (N107) - Combo? Fix?/Mod?: (contains duplij-23 (L23)) — Click to expand</summary>

     - Status:
     - Description: Capitalization check for the AI player has been improved. Now if AI has less than 4 units in the city, and also every three turns this mode will be reset and its necessity will be analyzed anew; slightly changed priority for several units that AI builds (decreased for Missiles, increased for Air Defense System and Converter); @GAPE tag in Game.txt about the presentation of a new technology has been corrected (for some reason, the code indicated that its appearance is active for AI players, I remade it for human players, the conditions for its appearance are: the presence of contact, the absence of war, alliance, embassy built by Marco Polo's Embassy and United Nations, the presence of an unresearched technology by the player); ransom received for captured barbarian leader has been reduced by half (paragraph 32).

     </details>

164. <details><summary>duplij-164 (N108) - Feature? — Click to expand</summary>

     - Status:
     - Description: Two variables have been added to the Rules.txt file in the @Cosmic tag, allowing you to set the number of points for movement on roads and railways. Air, hovering, and sea units do not participate in the calculation.

     </details>

165. <details><summary>duplij-165 (N109) - Mod? — Click to expand</summary>

     - Status:
     - Description: The ability for sea units to move along rivers has been added. Added the ability for cities located on a river, or near a river in an adjacent square, to build sea units. Three sea units (Carrier, Battleship, and Submarine) are excluded and will not be able to move along rivers, nor be built in cities on/near rivers due to their large displacement. Automation (AI) and movement by GOTO will now also take these capabilities into account. Any unit on board a transport (no matter whether air, ground, or sea) loses moves if the transport starts moving (barbarians and AI ignore this function), so for ground units, the transport must not move.

     </details>

166. <details><summary>duplij-166 (N110) - Feature? — Click to expand</summary>

     - Status:
     - Description: Two parameters that allow or prohibit leaving excess production in a city from previously built improvement/wonder or unit have been added to the Rules.txt file in the @Cosmic tag.

     </details>

167. <details><summary>duplij-167 (N111) - Combo? Fix?/Mod? — Click to expand</summary>

     - Status:
     - Description: Capitalization and Spaceship parts have been transferred from the category of wonders to the category of city improvements, which will affect the penalty when switching production; railways on ocean squares have been fixed, which are energy receivers, now they will not give a bonus of moves to units landing from sea transport (mercenaries have been added to the exception to have the ability to quickly attack).

     </details>

168. <details><summary>duplij-168 (N112) - Combo? Feature?/Mod?: (contains duplij-15 (L15)) — Click to expand</summary>

     - Status:
     - Description: Ability to transport units not only by sea or air transport, but also by land, has been added; one-time profit from organizing a trade route between cities has been reduced; value of some goods has been slightly changed (Rules.txt file, @Caravan tag); if there are trade routes between two nations and a war begins, profit from such trade routes will be reduced by four times during the war; city growth will now occur immediately as soon as food exceeds the limit without skipping a turn; a square with an airbase will now give only a basic resource, and not like a city; paratroopers can land from a Heavy Cargo Aircraft (HCA), provided that it has not moved during new turn; removed the ability to search for a city on a unexplored map due to lack of map centralization; coordinates and object number will not be displayed on unexplored territory; built units of Sputnik and UAV (Unmanned Aerial Vehicle) type will not cause unhappy citizens under Republic/Democracy (linked by line number at @UNITS tag, Rules.txt file); after researching Genetic Engineering, infantry units outside the city will recover +2 points to health faster; description of the unit recovery process has been added to the Civilopedia -> Game Concepts -> Unit Recovery; several errors and inaccuracies have been corrected.

     </details>

169. <details><summary>duplij-169 (N113) - Feature? — Click to expand</summary>

     - Status:
     - Description: Display of unit maintenance cost and overall balance has been moved to the F5 window for greater clarity; two new lines have been added to Labels.txt. Maintenance cost has been added to the Civilopedia when viewing Military Units. However, if you view units under different government systems, the cost will differ. It is worth noting that since maintenance under totalitarian governments is divided by two from the total amount, and when viewing unit maintenance cost, the division occurs from each unit, the final value may be slightly inaccurate (since the remainder from the fractional division will be discarded).

     </details>

170. <details><summary>duplij-170 (N114) - Mod? — Click to expand</summary>

     - Status:
     - Description: In the picture TERRAIN1.GIF, the image of the fish has been removed, and the old file has been saved with the mark fish (TERRAIN1_fish). Now, instead of a fish, there is a small 'wave'. In squares where there is a 'wave', it is preferable to build an underwater mine (it will give +1 shield).

     </details>

171. <details><summary>duplij-171 (N115) - Feature? — Click to expand</summary>

     - Status:
     - Description: A parameter has been added that allows you to specify the number of units that a carrier can transport and all units with the ability to transport missiles (in the Rules.txt file, @Cosmic tag).

     </details>

172. <details><summary>duplij-172 (N116) - Combo? Feature?/Mod? — Click to expand</summary>

     - Status:
     - Description: Another parameter has been added to the Rules.txt file, the @Cosmic tag, allowing use of the new unit exhaustion system. By default, this option is enabled (it can be disabled). A detailed description of how it works is in the Civilopedia -> Game Concepts -> Unit Exhaustion or at the end of the Describe.txt file. Sea and air units, with exhaustion enabled, can use fortification mode to recover; however, they will not receive an actual defense bonus from it.

     </details>

173. <details><summary><del>dupe: duplij-173 (N117)</del> - Feature? — Click to expand</summary>

     - Status: Duplicate; track via duplij-37 (L37)
     - Description: The ability to create huge maps at the start of the game, up to 65535 squares in size, has been added, as well as when creating a map in the Civ2Map.exe editor. Two options for choosing ready-made map sizes, "Very large" and "Huge", have been added. Parameters for creating maps have been slightly changed, so maps with a large selection of land mass will have slightly more land than before, provided that the "land form" is continents, and when choosing a small selection of land mass and "land form" is archipelago, on the contrary, there will be slightly less land than before.

     </details>

174. <details><summary>duplij-174 (N118) - Mod? — Click to expand</summary>

     - Status:
     - Description: The system of transporting missiles by different units with the missile carrier flag has been reworked. Now missiles can only be loaded in a city. When missiles are launched from a ship, its total capacity is reduced by each launched missile. If capacity is exceeded, any missile over the carrier ship will be destroyed on the next turn. To implement this mechanism, we had to add variables to the 'sav' save, so now saves of version 1.09 and below will not be able to load. AI ignores this mechanism and can still load and transfer missiles as before (in order that the rules for the player also apply to the AI, we look for the code 75 0E 8B 55 CC and change 75 0E to EB 0E, and to disable the new mechanism for a human, we change it to 90 90).

     </details>

175. <details><summary>duplij-175 (N119) - Combo? Feature?/Mod?: (contains duplij-38 (L38)) — Click to expand</summary>

     - Status:
     - Description: A variable has been added to the Rules.txt file, @Cosmic tag, prohibiting or allowing the destruction of units in a stack one by one, as in a city or fort (by default, the parameter prohibits destruction one by one). An ability to attack ground units in a stack, if they are covered by some air unit, has been added. The most protected unit will be selected first (a built-in procedure is used to find the best defender, no matter what type, similar to an attack on a city). If only air units remain in the stack, then they will no longer be able to be attacked by a ground unit. If an air transport, such as an HCA, hovers over land with units on board, it will not be possible to attack it, provided that the HCA is the main one in the stack, but if other units are in the stack, then such a block may not work.

     </details>

176. <details><summary>duplij-176 (N120) - Fix? — Click to expand</summary>

     - Status:
     - Description: A small fix for an exploit that allows attacking an empty city (without units) of any player with whom there is an agreement (cease-fire, peace, alliance) with nuclear weapons, without declaring war. Now, any air unit attacking an empty city of another player will first trigger a check for agreements; other adjustments and edits to some parameters.

     </details>

177. <details><summary>duplij-177 (N121) - Combo? Feature?/Mod? — Click to expand</summary>

     - Status:
     - Description: The global warming system, with the transformation of squares into other terrain types, has changed. Another climate change scenario has been added - global cooling. A number of conditions have been added to these scenarios that change the terrain of squares more smoothly with each new wave, forming areas of the same type. Ultimately, with global warming, with each new wave, there will be more and more deserts, and with global cooling, glaciers. A parameter has been added that allows, in addition to changing the terrain, to change part of the ocean coastal squares during global warming into oceans, during global cooling into glaciers, as well as a parameter that allows you to remove all pollution from the map after each wave of climate change. You can select a scenario and settings parameters in the Rules.txt file, @Cosmic tag. A more detailed description of the new climate change mechanism in Civilopedia -> Game concepts -> Global Warming/Cooling (advanced); @GLOBALCOOLING tag has been added to the Game.txt file.

     </details>

178. <details><summary>duplij-178 (N122) - Feature? — Click to expand</summary>

     - Status:
     - Description: A variable for the Unit Maintenance has been moved to a separate setting in the Rules.txt file, @Cosmic tag, where it can be enabled or disabled.

     </details>

179. <details><summary>duplij-179 (N123) - Mod? — Click to expand</summary>

     - Status:
     - Description: Value of a target for hitting with a missile has been redefined once again, reduced for a single Battleship, increased for a single Sputnik, and for ships carrying missiles on board. For an AEGIS Cruiser, if it is the main unit in a square, even with missiles on board, the value has been reduced, since hitting it with a missile is ineffective. If there are several ships or other attacking units in one square (for example, a transport will be loaded with attack units), this will increase the value of a target for hitting it with a missile in range.

     </details>

180. <details><summary>duplij-180 (N124) - Feature? — Click to expand</summary>

     - Status:
     - Description: Archive smac-cpu-fix-v1.0.zip has been added to the Distributiv folder. If you run the game through this program using the created shortcut with a parameter, for example, loader.exe -e civ2.exe, the processor load in Task Manager will be at level 0, i.e., the game will not fully load one thread/core.

     </details>

181. <details><summary>duplij-181 (N125) - Mod? — Click to expand</summary>

     - Status:
     - Description: The texture of Desert and Swamp terrain has been changed. The old version is saved under the name TERRAIN1_2.GIF.

     </details>

182. <details><summary>duplij-182 (N126) - Fix? — Click to expand</summary>

     - Status:
     - Description: A variable responsible for assigning a unit to a city has been changed. Now it is not a byte, but a word, which should solve a problem with the lack of a unit binding to its home city when the number of cities exceeds 255.

     </details>

183. <details><summary>duplij-183 (N127) - Mod? — Click to expand</summary>

     - Status:
     - Description: Capitalization income has been halved; now every two production shields give one gold, which increases taxes.

     </details>

184. <details><summary>duplij-184 (N128) - Feature? — Click to expand</summary>

     - Status:
     - Description: Several parameters in scenario editor settings have been adapted to the new game changes; the HELP.TXT file used when creating scenarios has been translated.

     </details>

185. <details><summary>duplij-185 (N129) - Fix? — Click to expand</summary>

     - Status:
     - Description: Text bindings have been fixed, font encoding, and fonts themselves when running GET_INFO.EXE from the Pedia folder have been changed. The translation that was used is the one that was already there (some pirated one is not quite accurate with a small edit; they have also already made edits to the code, changing English names to Russian ones with abbreviations to fit into the allocated section of the code). To run the 16-bit GET_INFO.EXE application, you will need to install the program from the otvdm-v0.9.0 folder -> install or install (no console). In fact, this Pedia skin is the same as Civilopedia in the game, but there are a few more graphics here. Skin is strictly tied to the parameters of the original game and does not expand with additional elements to the parameters of the modification, so it can be fully used with the original game only. Parameters of units, improvements, wonders, and terrain are loaded from the Rules.txt file of the game. Skin reacts to text lines in the Rules.txt file, and due to the large amount of added text, it did not load parameters. In connection with this, a more informative file with a detailed description was moved separately to Rules_full.txt, and the same file, but with a shorter description, was left as the main Rules.txt.

     </details>

186. <details><summary>duplij-186 (N130) - Combo? Fix?/Mod? — Click to expand</summary>

     - Status:
     - Description: Minor text edits; accidentally deleted Civ2Art.dll file returned, which allows displaying videos and correct display of a window with animation of the High Council; four more tracks from the Conflicts in Civilization add-on have been added.

     </details>

187. <details><summary>duplij-187 (N131) - Feature?: (also see duplij-26 (L26), duplij-27 (L27)) — Click to expand</summary>

     - Status:
     - Description: Playback of music tracks will now be interrupted during Consult High Council, diplomatic screen, playback of a video about a completed wonder, and exit to the menu. An additional trigger has been added to start a music track when pressing any command button or performing any action. A button has been displayed at the top of the map on the left, allowing you to randomly start another track, interrupting the previous one. If music is turned off via the @Cosmic tag in the Rules.txt file, the button will not be displayed.

     </details>

188. <details><summary><del>dupe: duplij-188 (N132)</del> - Mod? — Click to expand</summary>

     - Status: Duplicate; track via duplij-252 (SET9)
     - Description: A portrait of Stalin has been added instead of Lenin (file mk.dll). The leader's initials have been changed in the Rules.txt file, @leaders tag.

     </details>

189. <details><summary>duplij-189 (N133) - Mod? — Click to expand</summary>

     - Status:
     - Description: AI can now attack with nuclear weapons, not only cities, but also large stacks of powerful units (first, the damage to units from nuclear weapons is evaluated like a regular missile, and if requirements are not met, then damage to cities is evaluated). If there are friendly or allied units occupying 1 or 2 squares near the target, target attractiveness for attack decreases. If there are more than two friendly units and there are no secondary hostile targets, the attractiveness of the target is reset to zero, so that when a nuclear weapon explodes on enemy units, all friendly units are not destroyed. If there are other hostile units in the explosion zone, the attractiveness of the target will increase. If there is any city with an SDI Defense in a nuclear weapon attack zone (four squares from the center of the city with an SDI Defense), target attractiveness for a nuclear weapon attack is reset to zero, since the SDI Defense will almost certainly prevent the strike. Description "Target Attractiveness Evaluation" has been added to Civilopedia -> Game Concepts.

     </details>

190. <details><summary>duplij-190 (N134) - Mod? — Click to expand</summary>

     - Status:
     - Description: When attacking barbarian settlers, the reward for capturing the leader, which was given by default, has been removed. The reward has been left only when attacking a barbarian diplomat or caravan.

     </details>

191. <details><summary>duplij-191 (N135) - Combo? Fix?/Mod? — Click to expand</summary>

     - Status:
     - Description: The condition under which AI players changed their attitude towards another player if he dominated has been fixed. The limitation was set after the 250th turn (instead of the default 200th). Also, a condition for checking an alliance between the dominant player and another player has been added; if there is an alliance between them, then the check is bypassed. Thus, the player's attitude should not change towards a strong player if there is an alliance between them and one of them has moved to the dominant category. Added a description of "Domination" to the Civilopedia -> Game Concepts, with all the conditions that must be met for domination to work.

     </details>

192. <details><summary>duplij-192 (N136) - Combo? Fix?/Feature?/Mod?: (contains duplij-39 (L39), duplij-40 (L40)) — Click to expand</summary>

     - Status:
     - Description: Minor edits: The minimum value of "Communism is equivalent of this palace distance" item, that can be set in the Rules.txt file, @Cosmic tag, has been changed from 1 to 0, so that communism can be completely corruption-free; glacier and tundra have been removed from all sides of the map border, border is edged now only by the ocean (as a result, there is no glacier on random maps); probability of rivers appearing on the map during world formation has been slightly increased, in addition to the world size and humidity variables involved in calculating the number of rivers, a temperature variable has been added (by default, a river appears only on a plain/grassland, forest/jungle/swamp); icon display of the launch file in the upper left corner, when registering the main window, has been fixed. Now loading of the first ICON resource image is explicitly specified in the program (applies to all civ2 and civ2map exe files); a bug where the AI did not found cities well, or could not do it at all, has been fixed; names of cities founded by barbarians will be taken from the @EXTRA tag (CITY.TXT file).

     </details>

193. <details><summary>duplij-193 (N137) - Mod? — Click to expand</summary>

     - Status:
     - Description: Another attitude level, "Hateful", has been added to the list of possible relationships between players. It does not add any functionality; it simply reflects one of the ranges of negative attitudes between players. A corresponding line has been added to the Rules.txt file, in the @ATTITUDES tag. A description of "Attitude Level" has been added to Civilopedia -> Game Concepts, which allows you to better understand how this or that attitude is obtained.

     </details>

194. <details><summary>duplij-194 (N138) - Fix? — Click to expand</summary>

     - Status:
     - Description: Several word-sized variables have been converted to an unsigned state, thereby doubling the value that can be accommodated, and a check has been added to ensure that the maximum value of 65,535 is not exceeded. For example, this is a variable involved in calculating the strength of units (adding up the attack and defense strength of all of the player's units), and a population variable (adding up all of the player's city and population sizes). After an increasing number of cities, units, attack and defense values, and unlocking the population size, these variables could become negative and disrupt correct accounting and comparison.

     </details>

195. <details><summary>duplij-195 (N139) - Mod? — Click to expand</summary>

     - Status:
     - Description: Two more criteria have been added to the calculation of the relationship between players. Now, contribution to the relationship is made by the governments of the players and the total population of the cities. More details in Civilopedia -> Game Concepts -> Attitude Level.

     </details>

196. <details><summary>duplij-196 (N140) - Mod? — Click to expand</summary>

     - Status:
     - Description: Settlers are strictly forbidden to process ocean squares while on a boat (Settlers, Engineers and Converter obey the rules in the Rules.txt file, @TERRAIN tag -> Irrigate/Mine is set to 'yes' then processing is possible, if 'no' then no, if there is a tag of another terrain type, then it is possible to transform on it). Settlers are prohibited from processing the 'yes' tag on the ocean and building energy receivers; the rest is left as it was. Engineers can process ocean squares only while on a boat.

     </details>

197. <details><summary><del>dupe: duplij-197 (N141)</del> - Feature? — Click to expand</summary>

     - Status: Duplicate; track via duplij-42 (L42)
     - Description: Support has been added for images that can be displayed in the Civilopedia when viewing Civilization Advances, City Improvements, Wonders of the World, and Military Units. Images are in the Pictures folder. The supported image format is 256-color GIF. More details in the Pictures folder, !read.txt file.

     </details>

198. <details><summary>duplij-198 (N142) - Feature?: (also see duplij-43 (L43), duplij-44 (L44)) — Click to expand</summary>

     - Status:
     - Description: The window used in the Civilopedia has been slightly increased vertically from 640x400 to 640x480. Display parameters for text lines of titles have been adapted to the new window sizes, taking into account the output of 12 lines instead of 9. The Improvements.gif file has been added to the Pictures folder, from which small images will be taken in Civilopedia, and sometimes in the game itself during certain events or actions. When using this file, a window in Civilopedia will be increased to 700x600 in order to better accommodate the new images. If you delete this file, the game will only use thumbnails from the ICONS.GIF file with a window size of 640x480, so the player can choose between the old look and the new one.

     </details>

199. <details><summary>duplij-199 (N143) - Combo? Feature?/Mod? — Click to expand</summary>

     - Status:
     - Description: A parameter that allows you to set financial assistance to AI players has been added to the Rules.txt file, @Cosmic tag. After I enabled the AI improvement maintenance cost, its game quality dropped; this parameter can help. The parameter allows you to add gold to AI players depending on the sum of indicators (turns completed, number of cities and their size, units and their power) adjusted for odds and divided inversely proportional to the difficulty level of the game. More details in Civilopedia -> Game Concepts -> Complicator or in the Describe.txt file.

     </details>

200. <details><summary>duplij-200 (N144) - Combo? Fix?/Feature?/Mod?: (contains duplij-8 (L8)) — Click to expand</summary>

     - Status:
     - Description: Two parameters 'Treasury' and 'Final Balance' have been moved to the Wealth Distribution window (Shift+T) for greater clarity and assessment of the situation when distributing budget allocations; auto-close time for many windows has been increased; colored pixels have been added to explosion animation during the battle (ICONS file); the Elite line and general support for up to 910 text lines have been added to the Labels.txt file; the @CITYRAZE, @BOND0072 and @PROMOTED2 tags have been added to the Game.txt file; an inaccuracy has been fixed where a spy with an invisibility flag could take missiles from the city; empty barbarian ships far from the city will no longer stand still for a long time, but will immediately disappear; city support for specialists has been increased to 48; behavior of AI transport ships when transporting units has been improved; several checks have been added to improve the movement of AI units on the map; other various fixes.

     </details>

201. <details><summary>duplij-201 (N145) - Mod? — Click to expand</summary>

     - Status:
     - Description: City can now process four more squares (a total of 25 squares, including the central one). Added squares have reduced sensitivity to mouse clicks; only half of the squares closer to the common center are sensitive to clicks. An image that will be loaded from the ICONS file, when the same square is occupied by one of the cities in the visibility zone of two cities, has been changed. Namely, the procedure that loads the empty area with number 8 has been redesigned to load a rectangle with the inscription BUSY.

     </details>

202. <details><summary>duplij-202 (N146) - Mod? — Click to expand</summary>

     - Status:
     - Description: A continuation of the upgrade that the unit can receive after a battle has been added. Now, after receiving a "Veteran" status, the unit can receive an "Elite" status in the following battles. If a Veteran gives 25%, then an Elite increases the attack and defense indicators to 50% compared to the base value. New status can also be used by the Spy with an increased chance of success; the cost of bribing a damaged unit will decrease depending on the amount of damage. The cost of bribing a veteran has increased by 25%, elite by 100%. More details in Civilopedia -> Game Concepts -> Veteran Units and Elite Units item, or in the Describe.txt file.

     </details>

203. <details><summary><del>dupe: duplij-203 (N147)</del> - Feature? — Click to expand</summary>

     - Status: Duplicate; track via duplij-250 (SET7)
     - Description: The check that prohibits deleting your city, if cheat mode is not active, has been removed. Now, an empty city can be deleted like a unit, using a key combination (Shift+D), but this will affect the reputation if deletion occurs when the cheat mode is not active. The size of the destroyed city divided by 8 will give a value that will be added to the reputation, worsening it.

     </details>

204. <details><summary>duplij-204 (N148) - Combo? Feature?/Mod? — Click to expand</summary>

     - Status:
     - Description: A mechanism for selecting your unit from a square in which an ally unit is also located has been improved. Now the player's unit will automatically become first in the list when you right-click. Now all units located in the same square, even those of different players, will be displayed in the left window of the information panel when you select at least one or hover over it. Transport ships can now transport allied units. To do this, the allied unit must be stationary in the square and must have the S order. When moving an air transport to it, the unit will be automatically loaded on board. Similarly, you can send your unit to an ally's sea transport near the shore. Such transportation does not provide visibility of movement after loading, since the leading unit-carrier has the main view, so you will have to track your unit yourself, as a passenger; visibility of movement of allied units has been added along those sections of the map that were previously opened by the player (to speed up the movement, you can press and hold SHIFT during a turn of other players); visibility of mercenaries/barbarians movement has been removed (except for the first two easiest difficulty levels), now any enemy units will be visible only next to player's units, or in open areas of the map near cities at a distance of three or less squares (four squares after researching Mobile Forces technology).

     </details>

205. <details><summary>duplij-205 (N149) - Feature? — Click to expand</summary>

     - Status:
     - Description: The ability to lay new riverbeds from existing rivers has been added. For this purpose, a corresponding option has been added to the Orders menu, and this order can also be called by pressing the N key. The @RIVERBED1 and @RIVERBED2 tags have been added to the Game.txt file, and a line has been added to the @ORDERS tag of the Rules.txt file, a line has been added to the Menu.txt and Labels.txt files. More details in Civilopedia -> Game Concepts -> Riverbed Laying.

     </details>

206. <details><summary>duplij-206 (N150) - Feature? — Click to expand</summary>

     - Status:
     - Description: The array for storing the results of automatic movement of the AI unit and the player by the 'Go To' command has been increased. Thanks to this, it was possible to significantly improve the movement and reduce bottlenecks, since the calculation area of the calculated route has increased significantly. To increase the array, it was necessary to increase the size of executable files. More details about the operation of this mechanism in Civilopedia -> Game Concepts -> Auto-movement.

     </details>

207. <details><summary>duplij-207 (N151) - Feature? — Click to expand</summary>

     - Status:
     - Description: When 'Customize World' is selected, two options have been added to the 'Temperature' category: Cold and Hot. When choosing Cold, the generated world will have a lot of glaciers in the north and south (on small maps, glaciers can extend all the way to the equator). A Hot world will have no glaciers at all and will almost certainly have no tundra. A Warm world will almost certainly have no glaciers, but will have some tundra. Temperate and Cool worlds will differ in the amount of glaciers. High humidity in all worlds significantly increases the amount of vegetation and swamps, which reduces the amount of grassland and removes deserts. Two lines have been added to the Game.txt file, @CUSTOMTEMP tag.

     </details>

208. <details><summary>duplij-208 (N152) - Feature? — Click to expand</summary>

     - Status:
     - Description: The civ2hc.exe executable file with support for 24-bit colors and BMP files has been moved to a separate HighColor folder. This is a test version created by me, by adapting and transferring these procedures here from the Test of Time game. A more detailed description of all changes and additions is in the !readme.txt file from this folder.

     </details>

209. <details><summary>duplij-209 (N153) - Feature? — Click to expand</summary>

     - Status:
     - Description: The number of continents, archipelagos, islands, and large lakes that are created at the start of the game has been increased from 64 to 255. This increase should improve the processing and movement of AI on them. This number almost completely covers all objects when creating a maximum world with a land form Archipelago.

     </details>

210. <details><summary>duplij-210 (N154) - Mod? — Click to expand</summary>

     - Status:
     - Description: A number of parameters when 'Customize World' have been changed. For the Wet Climate, vegetation in the created world has been significantly increased, Jungle in the Warm Temperature world, and Forest in the Cold Temperature world, and the number of Swamps has been increased, the amount of Tundra will be reduced, and some of it will be replaced with Forest. In the created world with Wet Climate, almost all lakes in one square will be replaced with land terrain; the type of land terrain will be selected from the adjacent square to the left or right side of the current square. In the Normal Climate world, this will also affect two-square lakes, and in the Arid Climate world, three-square lakes; thus, the created world will acquire a more realistic appearance, and having access to water will become a significant condition for development.

     </details>

211. <details><summary>duplij-211 (N155) - Mod? — Click to expand</summary>

     - Status:
     - Description: The RPG unit has been replaced with the Radar (Radio Detection And Ranging). This is a no-move unit, capable of detecting any unit, including invisible ones, within three squares of it. More details about its properties can be found in Civilopedia -> Military Units -> Radar -> Info -> Description. Since many units have their own individual properties, more detailed information about them can be found in the Units Description section.

     </details>

212. <details><summary>duplij-212 (N156) - Combo? Feature?/Mod? — Click to expand</summary>

     - Status:
     - Description: The ability to set the Long-range Strike attribute to units has been added. For this purpose, a corresponding column has been added to the @UNITS tag of the Rules.txt file. More details about this mechanism can be found in Civilopedia -> Game Concepts -> Long-range Strike. In this version, the unit's hit points and firepower variables have been added to the final calculations for choosing who will strike in a battle, the attacking or defending unit, in addition to the attack and defense factors. This will probably change the result slightly, since, for example, a damaged unit will be slightly weaker than before, and a unit with high firepower will become stronger in any role. More details on this in Civilopedia -> Game Concepts -> Battle Calculation.

     </details>

213. <details><summary>duplij-213 (N157) - Feature? — Click to expand</summary>

     - Status:
     - Description: A variable for storing city images has been moved to a new address space, thereby expanding the number of possible city types used in the game to seven. The last type, called MODERN NEW (CITIES.GIF/BMP file), is now available in the game and will be activated after researching technology - New Technologies.

     </details>

214. <details><summary>duplij-214 (N158) - Feature? — Click to expand</summary>

     - Status:
     - Description: Several types of random events have been added. More details in Civilopedia -> Game Concepts -> Random Events. You can disable events or change the probability of occurrence in the Rules.txt file, @Cosmic tag. A number of tags starting with @EVENT have been added to the Game.txt file. Several lines have been added to the end of the Labels.txt file. The EVENTS.GIF/BMP file containing images of events is displayed separately. The probability of state events is on average 1% per turn; it can be edited separately. To do so, find the following code: 83 C0 00 3B D0, and instead of 00, put, for example, 03, which will double the probability, and a negative number will reduce or completely exclude it.

     </details>

215. <details><summary>duplij-215 (N159) - Combo? Fix?/Mod? — Click to expand</summary>

     - Status:
     - Description: Other fixes: A visible overview at the start of the game is brought into line with the new city boundaries; white frame of the city boundaries when activating Show Map Grid is expanded to the new city sizes, taking into account the additional squares; barracks in the city will no longer fully restore a damaged ground unit on the next turn, if unit has a damage level of approximately more than half of its Hit points; units with an invisibility flag cannot establish contact, the same applies to units with domain 3 (except for the Converter); the Converter can now be transported by Transport, same applies to the Radar; if a hidden infantry unit has a Ignore zones of control flag (spy), then any ground unit relative to it will also not have zones of control, thus it will be more difficult to detect the invisible unit; the AI will now build an Explorer (previously, the last slot where it was moved, did not participate in calculation of the unit selection for construction); in ICONS.BMP file of the HighColor folder, areas of frames that are loaded in the game, but not used, are marked with numbers (a mark is needed so that a warning about an incorrect color table does not appear when they are loaded in the smeds.log file when starting civ2hc version; a number of SendToConnections: FAILED on send to connection warnings that can be displayed in the smeds.log log has been significantly reduced (warnings for a section of the network game that should not be active in a regular game); remaining music tracks from the Expansion Packs have been added; autosaves will now be placed in the Save folder by default; brief information about the modification, in form of vertical moving lines from the modific.txt file, has been added to Civilopedia -> About Civilization II; picture of Fortress and Airbase has been changed.

     </details>

216. <details><summary>duplij-216 (N160) - Combo? Fix?/Mod? — Click to expand</summary>

     - Status:
     - Description: Several errors have been fixed (fixed placement of citizens in the city on the central and processing squares when city's population grows or decreases; random event variables moved to a new location, since previous ones could be erased, giving an empty window; ability to expel spies and diplomats from your territory, if there is allied relationship between the players, has been returned); the procedure for bribing cities has been redesigned, with an increase in cost, depending on the presence of improvements, wonders, units and some researched technologies in the city, the procedure for bribing units has been improved. If a nation of the bribed city has Anarchy form of government, cost of bribing will be lower, and if Fundamentalism, then higher; a detailed description of bribery has been added to Civilopedia -> Game Concepts -> Bribery; when bribing or capturing a city, the remaining units from this city will remain at the disposal of the previous owner if they are located at a distance of no more than two squares from the city, while their support from this city will be removed; file WONDER05.AVI (Oracle wonder) has been replaced with a corrected one (previous one could have displayed a frame unpacking error message while playing the game); text limit in Describe.txt file expanded from 4095 to 10240 characters in a single @@ tag.

     </details>

217. <details><summary>duplij-217 (N161) - Feature? — Click to expand</summary>

     - Status:
     - Description: The ability to install the English language has been added (thanks to comrade Slob Humar/slobi, who spent time on translation).

     </details>

218. <details><summary>duplij-218 (N162) - Combo? Fix?/Feature? — Click to expand</summary>

     - Status:
     - Description: Several bugs have been fixed (fixed display of resource collection in the city on the central square when converting all citizens to specialists, with subsequent starvation and decreasing size of the city; fixed retention of units transported in water (ocean/river square) by AI transport ships, if they switch to fortification mode on board; fixed game crash when @FINDCITY window was opened while there were more than 535 cities in the game; fixed an error in Game.txt file, @CITYRAZE tag, with the lack of outcome choice; fixed an error introduced in version 1.16 after increasing the count of islands and continents, which could lead to deterioration of AI behavior); in Random Events, Fortress will no longer be created on ocean squares. 'Positive' improvements for Random Events can now appear on squares with a river; improvements that AI players periodically receive on squares without actually processing them, will no longer appear in the Ocean, except when this happens in Random Events; variable responsible for the counter of selecting the name of a new city from the CITY.TXT file has been expanded, now it is not a byte, but a word. When exceeding 255 cities, the list will not start over, but will be continued if there is a free text list of city names; zeroing of the variable responsible for selecting name of a new city from the CITY.TXT file has been added, now after exiting the current game in the menu and starting a new game, new cities will start from the beginning of the list, and not from the continuation of the list from the previous game; if you buy Settlers at city size 1, then the resulting settlers will always be without binding to the city, and the city itself will be disbanded; received unit in the Hut will always be without binding to the city; if in random events, that affect the population size, decrease exceeds the size of the city, then you can set the outcome scenario, either city is destroyed, or remains with a negative size and zero support, but can quickly grow to a positive size (Random Events parameter in Rulex.txt file, @Cosmic tag); the variable that takes into account existing, under construction and destroyed units, has been expanded from a byte to a double word (when pressing F2 Statistics/Casualties), now the values will not be reset when reaching 255; removal of a worker from a water city square has been added to the barb files, when an enemy unit, such as a ship, is in the city limits; the scenario editor has been improved to correctly account for the added variables when transferring - Long-range Strike, MODERN NEW (Ultramodern) city style and Domain 3 for units, the transfer of added effects from the Rules.txt file, @Cosmic tag remains unrealized, they need to be transferred to the created scenario manually; autosave has been improved, if it's a regular game, saves will now be written to the Save folder, and if a scenario, to the running Scenario folder. Support for searching BMP/GIF files not only in the current folder, but also in the root folder, has been added. (27/04/25) An error with establishing contacts when meeting ground units has been fixed. Contacts can be made by all ground units (except Spy), Converter, and sea units when encountering a city. A bug in the original game, where an air unit could contact a city in which there are no units, has been fixed.

     </details>

219. <details><summary>duplij-219 (N163) - Mod? — Click to expand</summary>

     - Status:
     - Description: The procedure that marks squares suitable for founding a city in the generated world has been changed. In addition to grassland and plains, it now takes into account forest, hills, tundra, jungle, and swamp. The presence of resources within a radius of two squares from the center now significantly increases the attractiveness of the central square for founding a city. The procedure for selecting starting positions of players has been adapted to the new markers and will better take into account resources within a radius of two squares, especially if there are few plains or grassland in this radius. The starting settlers can now appear on all the squares specified before. If controlled by a human, the starting settlers unit is given the opportunity to found the first city in a forest, tundra, jungle, or swamp with automatic conversion of the central square to a plain or grassland. For AI settlers, this opportunity is given for all new cities, and for the first city, the conversion will affect all areas within a radius of 1 square.

     </details>

220. <details><summary>duplij-220 (N164) - Feature? — Click to expand</summary>

     - Status:
     - Description: An option which allows you to adjust the created world - increase or decrease land area, create more fragmented and winding land areas or ordinary ones in the form of islands/continents, remove internal lakes or not, has been added to the Rules.txt file, @Cosmic tag. When choosing land form - Archipelago, there will be more mountains if the age selected is 3 billion years or hills if 4 billion years is selected, plus small adjustments to the parameters during general creation of the map.

     </details>

221. <details><summary>duplij-221 (N165) - Mod? — Click to expand</summary>

     - Status:
     - Description: A number of conditions for Diplomats, when conducting operations in the city, have been changed. Now, Diplomats, like Spies, when attempting to Steal Technology, will not have complete carte blanche to steal technology, but there will be a chance that they will be captured, both before and after the operation. If the operation is successful, he will be able to return home and advance in rank. When conducting operations such as Steal Technology, Industrial Sabotage, Poison Water Supply, Plant Nuclear Device (Diplomats can't Poison Water Supply or Plant Nuclear Device), if there are enemy Diplomats or Spies in the city, the probability of breaking through them to carry out the operation will decrease, and the more of them there are in the city, the lower the probability will be. If your agent is spotted by enemy spies, they will most likely have to return home to avoid detection. More details in Civilopedia -> Game Concepts -> Conducting Operations or in the Describe.txt file.

     </details>

222. <details><summary>duplij-222 (N166) - Feature? — Click to expand</summary>

     - Status:
     - Description: Support for larger size units up to 64x64 pixels has been added to the civ2hc.exe file, similar to TOT. It is possible to replace current unit images in the UNITS.BMP file with your own enlarged ones, saving the file in 24-bit BMP format. You can use any program, such as the built-in Paint. The flag position near the unit is determined by the intersection of imaginary lines from two blue pixels on the frame border at the top and left (left edge of the flag). It is recommended not to shift the blue dot too far to the right (no closer than 20 pixels to the right border and 6 pixels to the left border). Otherwise, if there are several units in a single square, an enlarged flag may not fit the field border and will be cut off. To better accommodate large unit sizes, various adjustments have been made in many windows where they are displayed, increasing the distance to the image borders. The civ2hc.exe file can only work with a UNITS.BMP file enlarged to 640x640 pixels, or with a standard UNITS.GIF file at 640x480 pixels, while unit flags and their colors are taken from the same file. This file has been updated to beta4.

     </details>

223. <details><summary>duplij-223 (N167) - Feature? — Click to expand</summary>

     - Status:
     - Description: The number of technologies in the game has increased to 126. While these technologies are currently disabled, they can be enabled and configured by editing the parameters in the Rules.txt file under the @Civilize tag. Additionally, a description can be added in the Describe.txt file.

     </details>

224. <details><summary>duplij-224 (N168) - Feature? — Click to expand</summary>

     - Status:
     - Description: The scenario editor has been updated to accommodate the new total of technologies and will now accurately reflect them. Improvements have been made to ensure the correct transfer of effects from the Rules.txt file to a new scenario (@Cosmic tag), which is now fully functional within the modification. Please note that the game has a limit on the latest technologies that can be researched by the AI. Therefore, if new technologies are introduced and expected to be discovered by both human players and the AI, they must be placed in the hierarchy before the Energy Transition and Mobile Warfare III technologies. The civ2hc.exe file has been updated to beta5.

     </details>

225. <details><summary>duplij-225 (N169) - Feature? — Click to expand</summary>

     - Status:
     - Description: The total number of units has been increased from 4521 to 5226. This includes approximately 5126 of all units and 100 of non-military units only, achieved by transferring variables

     </details>

226. <details><summary>duplij-226 (N170) - Combo? Fix?/Feature?/Mod?: (contains duplij-47 (L47), duplij-48 (L48), duplij-52 (L52)) — Click to expand</summary>

     - Status: duplij-47 (L47): OC2 code appears not affected, would need more details or a recreate or a .SAV file to dig deeper; see https://github.com/richardpenman/civ2/issues/24#issuecomment-6817643124; still unresolved: the other fixes and features (Civilization Advances tree, Metrology rename and epochs, TUTORIAL.SAV removal, bold BUSY, Seal renamed Walrus, credits and title images, red UNHAPPY message, red disorder flag, larger High Council window, civ2hc colors, show allies' moves option, EDITTECH Cancel fix, Describe.txt fixes)
     - Description: Several bugs have been fixed, including the issue with the Civilization Advances tree, which is now functioning correctly. The technology previously known as Precision Measurement has been renamed to Metrology. Additionally, the technological epochs for both the Machine Tools and Metrology technologies have been corrected. The TUTORIAL.SAV file has been removed, as it was incompatible with the mod; the tutorial scenario is based on this file. The tutorial is available in any game by enabling the "Tutorial Help" option in the Game Options menu. The word BUSY displayed in the city's Resource Map is now bolded, and the black frame surrounding this square has been removed. The name of the Seal special resource has been changed to Walrus. New ACREDITS and SCREDITS images (in BMP and GIF formats) have been added when selecting About Civilization II in the Civilopedia menu and viewing internal options (View Credits). A TITLE image (also in BMP and GIF formats) has been added to the initial game launch window. These files are optional and can be deleted if desired, restoring the previous view. The city window will now display a red text message reading "Citizens are UNHAPPY!" instead of just "Citizens" when the number of dissatisfied citizens surpasses the number of satisfied citizens. This change makes it easier to identify when a city is experiencing disorder. The color of the disorder city flag on the world map has been changed back to red. Slightly increased the size of the "Consult High Council" window and enlarged the font size of the displayed text. The color in various windows was adjusted when using the civ2hc.exe file. A negative ID error that occurred when the entire map was revealed, specifically when a unit was positioned at coordinates 0, 0, has been fixed. This issue previously caused map distortion or crashes. A "Show allies' moves on the Open Map." option has been added to the Game Options, allowing you to show or hide allied moves on player-opened map sections. The accounting for technologies selected through the EDITTECH editor has been adjusted. Previously, selecting technologies individually, then clicking the "Give/Take All" button, and then closing the window by clicking "Cancel" would duplicate unlocked technologies. The variable accounting for the player's unlocked technologies would display incorrect values until updated. Now, clicking "Cancel" will completely recalculate unlocked technologies. Finally, the descriptions for certain variables in Describe.txt have been corrected for both the Russian and English versions.

     </details>

227. <details><summary>duplij-227 (N171) - Combo? Fix?/Feature?/Mod?: (contains duplij-46 (L46)) — Click to expand</summary>

     - Status: duplij-46 (L46): fixed previously in OC2; see https://github.com/richardpenman/civ2/issues/24#issuecomment-6817643124; still unresolved: Uranium supply/demand changes, renamed goods (Meat, Jewelry, Fuel), trade-offer generation from city terrain, Food route limit and amount, cancelling trade routes
     - Description: A fix has been implemented to accurately calculate trade goods, such as dye and copper, in the Demand category. Uranium can now appear when determining the second good in the Supply list based on city coordinates. Some product names have been changed to be more general, such as Beads being renamed to Meat, Gems to Jewelry, and Oil to Fuel. Uranium demand has been reduced tenfold, yet it remains at a significantly elevated level. Additionally, the mechanism for generating trade offers in cities has been updated. The list of trade offers will now be influenced by the resources within the city's borders, as well as the type of terrain within them. For more details, please refer to the Civilopedia under Game Concepts -> Trade and Trade Routes. The number of Food trade routes a city can accept from other cities is now limited to 10 (the limit is tied to the variable "# of rows in food box \* 2"). The amount of food a Food route will transfer has been increased from 1 to 2. If a city's surplus is less than 2 units of food, the created merchant in that city will not have the option to select Food Supplies as a trade good. The ability to cancel trade routes in cities has been added. To do this, click on the trade route in the city window and select the desired option in the new window.

     </details>

228. <details><summary>duplij-228 (N172) - Mod? — Click to expand</summary>

     - Status:
     - Description: Huts (settlements) will no longer disappear when an air unit flies over them or a ship sails along the river; only ground units with a domain of 0 and roles 0, 1 (military units), and 6 (agents) will be able to interact with the huts. If a hut is located within city borders, it cannot be razed by moving workers. While the hut remains standing, the city square that contains it will not provide resources. This settlement will occasionally provide you with units as a form of patronage. These units require no upkeep and have a 50% chance of being veterans. These units can only be ground units, depending on the knowledge available worldwide. The maximum rank of a unit can be either Marines or ADS, and the selection is random. The frequency with which a settlement can produce units is determined by the @Cosmic tag setting in the Rules.txt file, which has two options: turn 15 or turn 31. If units are not needed, the settlement can be destroyed, which will grant the standard bonus offered by the game and make the square available. A new feature has been introduced that allows agent units (both diplomats and spies) to interact with a hut. If they possess the required amount of gold, these agents can transform the hut into a city of random size, ranging from 1 to 4, and with randomly assigned improvements such as a Temple, Granary, MarketPlace, or Library. The cost for this transformation depends on the number of turns that have been played, the number of technologies researched by the player, and the city size. If a settlement is closer than the minimum distance from an existing city, as specified in the @Cosmic tag settings, the agent's interaction menu with that settlement will not appear. Please note that AI players will not utilize this feature. Additionally, the tags @ENTERVILLAGE, @BLDVILLAGE, and @CANCELTROUTE have been added to the Game.txt file. In the barb files, both barbarians and mercenaries will now be able to obtain items from huts in the same way that players can.

     </details>

229. <details><summary><del>dupe: duplij-229 (N173)</del> - Combo? Fix?/Feature? — Click to expand</summary>

     - Status: Duplicate; track via duplij-253 (SET10)
     - Description: Fixed the issue with technology where unlocking Tactics should grant Cavalry units in huts. Corrected the information window display to show the unit icon properly after unlocking Explosives with an Engineer, or Robotics with a Converter. Empty barbarian ships are now deleted immediately to prevent repeated landing messages when they linger near a city. Added the option to declare war through diplomacy, provided the player is not under a Republic or Democracy government, and the opponent does not have the Great Wall or United Nations wonders. Alternatively, if the player controls the United Nations, war can also be initiated with Senate approval, regardless of government type. The AI may attempt to appease you by offering tribute, such as gold or technology. You can choose to accept this offer or reject it by declaring war. The @DIPLOMACYMENU tag has been updated with a new entry, and the tags @THREATWAR(2) and @THRWAR(0,1,2) have been added to the Game.txt file; the @ALMOSTWONDER message has been updated to accurately reflect the production bonuses the AI receives.

     </details>

230. <details><summary>duplij-230 (N174) - Mod? — Click to expand</summary>

     - Status:
     - Description: A new resource, Construction Materials, has been introduced. This resource is specifically used to accelerate the completion of Wonders of the World. Other types of goods and cargo, such as those delivered by caravans or freight, cannot be used to expedite these projects.

     </details>

231. <details><summary>duplij-231 (N175) - Combo? Fix?/Mod? — Click to expand</summary>

     - Status:
     - Description: AI construction priorities for Mass Transit, Recycling Centers, and Solar Plants have been slightly increased; the clickable area for selling buildings in the city window has been narrowed; the condition for unilaterally terminating agreements has been updated; a new check of the player's relations level (when possessing the UN) ensures the @ACCURSEDUN message is not always shown during diplomatic contact; saving and loading of discovered technologies is now supported, so the F6 window accurately records and highlights the first discoveries in white color; background music volume has been slightly reduced; after the first warming event, water improvements such as aquazones and aquafarms are now only partially removed rather than entirely erased. Terrestrial improvements are eliminated from all squares that undergo terrain changes (for example, a grassland converted to a swamp will lose its farm and irrigation); the irrigation texture will now be applied before the river texture, rather than after, and will thus not hide the river on the landscape; dithering on the border between open and hidden map areas (black spots) has been removed

     </details>

232. <details><summary>duplij-232 (N176) - Combo? Feature?/Mod?: (contains duplij-49 (L49)) — Click to expand</summary>

     - Status:
     - Description: The white highlighting frame on the map can now be moved with the keyboard even after all units have completed their turns; In random events, when a city is destroyed, a settler (engineer/converter) will now be able to appear on the site of the destroyed city; The connections and interdependencies for a number of @Civilize technologies have been changed in the rules.txt file; The probability of catching enemy spies when committing various types of sabotage in the cities of players with Communism and Fundamentalism will be slightly higher; The city's accumulated production, which could remain after the completion of a project (wonder of the world) by other players, will no longer be reset, but there will be a gradual decrease in accumulated shields until the production is changed to a new one or capitalization; Airships and Helicopters can be attacked if they missed a turn and did not move, thus it will be assumed that they landed this turn; Added technology tree file tech_tree.png (item 176 added 14/02/26)

     </details>

233. <details><summary>duplij-233 (N177) - Combo? Fix?/Mod? — Click to expand</summary>

     - Status:
     - Description: Several bugs have been fixed - stuck AI missiles near cities will now move into the city. Fixed the Helicopter spawning for mercenaries before players have unlocked the necessary technologies. Fixed the spawning of a pillbox unit for mercenaries on land; Added the ability to enter your city if an allied unit is located there; Added a description of how a target for a nuclear attack is selected to the Civilopedia, under the "Target Attractiveness Evaluation" section; The sale price of constructed buildings has been reduced by 10%, and the income from the sale will now be 90% of the invested shields instead of 100%; Network variables responsible for units have been increased to the new effective unit limit (5226 units); When the map scale is reduced to -3 or less, the rendering of units while moving will be disabled to avoid graphical distortions

     </details>

234. <details><summary>duplij-234 (N178) - Mod? — Click to expand</summary>

     - Status:
     - Description: Added an implementation for radiation sickness and quarantined cities after they have been attacked by a nuclear missile or subjected to nuclear sabotage. A quarantined city will generate no income and will be unable to grow food, with production reduced by a factor of four. This state can last from one to 15 turns. A mushroom cloud will hover over the quarantined city on the map. In cheat mode, this flag can be removed by opening the city editor. The @RADIATIONSICK tag and text string have been added to the @CITYEDIT tag in the game.txt file

     </details>

235. <details><summary>duplij-235 (N179) - Feature? — Click to expand</summary>

     - Status:
     - Description: The maximum number of cities has been increased from 626 to 857 by moving variables to increase the address space for cities. The maximum number of military units for each AI player has been increased from 812 to 1000, after which the AI player will switch to building structures or capitalization

     </details>

236. <details><summary>duplij-236 (N180) - Fix? — Click to expand</summary>

     - Status:
     - Description: A bug with the missing @RADIATIONSICK tag in the Game.txt file has been fixed; a crash when creating a script in a scenario with the indication of the output of a text message TEXT/ENDTEXT has been fixed; a byte overflow responsible for the number of moves for a Sputnik after an attack has been fixed. Now, as it should be, Sputnik will be able to make up to two attacks per game turn, each attack removes half of the remaining moves; the ability to attack Sputnik, as well as other air units, has been added to ICBMs; fixed an error in the production of traders that sometimes led to the deletion of units due to an incorrect calculation of the total production of the city (added production bonuses from improvements were not taken into account) when comparing it with the cost of maintaining units; fixed an inaccuracy, when a heavy cargo aircraft (HCA), flying over Converter located on a water, transferred it to its hold. Now, as it should be, to transfer Converter to the hold, it must be in the Sleep (S) mode; a converter located on the water can now be given the 'S' order to sleep; fixed an inaccuracy in the display of the number of turns required to complete production in a city in the presence of a nuclear mushroom cloud (the displayed turns did not take into account the decrease in production from radiation sickness); fixed an inaccuracy that in rare cases caused a message about violating city borders, although in fact there was no violating unit present; corrected the display of the relationship level in diplomacy; now instead of the 'Receptive' relationship level, 'Reserved' will be written in cases where the actual digital level was lower than the displayed symbolic one; fixed an inaccuracy that sometimes caused leader's name to be missing in the @LOADOK tag window after loading the game; fixed an incorrect definition of the condition when selecting the message display between the @PLEASECITIES and @PLEASECITY tags; Fixed a bug where there was no notification when a unit was destroyed due to exhaustion ATTRITION; slightly adjusted the mechanism for founding a city by AI players; slightly increased the priority of building defensive units in AI cities.

     </details>

237. <details><summary>duplij-237 (N181) - Mod? — Click to expand</summary>

     - Status:
     - Description: The maximum number of units each AI player can have before entering Capitalization has been adjusted to 100 units plus 20 per city, up to 1800, so larger players can have more units than before; a check for the displayed ending of the words "khod/khoda/khodov" has been added when constructing units or improvements in a city. This check is performed up to 80 turns. If construction takes longer, the word "khodov" will always be displayed; the mechanism by which AI players spontaneously attack human players has been improved. Now, only AI players of a similar level or stronger than the human player will use this, rather than weaker ones, as was previously the case. The main triggering factors are the presence of the Vendetta flag, a relationship level of "Uncooperative" or worse, the player's reputation, frequent encounters between the AI player's and the human player's units, and randomness. This check is performed every three turns; another probability of AI attack has been reduced, in addition to the one previously described. In this case, the AI can be weaker than the human player, but not significantly, and the relationship must be Hostile or worse; players are prohibited from settling on squares containing a hut.

     </details>

238. <details><summary>duplij-238 (N182) - Mod? — Click to expand</summary>

     - Status:
     - Description: The ability to attack a player's unit has been added when multiple units from different players occupy the same square (for example, if they are allied). The unit the player is at war with will be selected first. If none are present, the first identified player with whom the player has a peace treaty or truce will be selected. If this player has a peace treaty, the player will be prompted to terminate the treaty. After destroying one unit of such a player, one or more units of players with whom the player is at war will effectively disappear from the map. If there are war statuses with multiple players, all their units will disappear from the square, and the strongest of them will defend. The same rule applies: all units on the same square are destroyed after defeating one of them (except when units are located in a fortress). This implementation will prevent accidental declarations of war when one of the AI players attacks an enemy unit that was allied with a human player and whose unit was also on the same square. If two players' cities are located near the intersection of some squares, the presence of units on such squares will not be considered a border violation, except when they are on a square adjacent to (or right next to) a city.

     </details>

239. <details><summary>duplij-239 (N183) - Mod?: (contains duplij-53 (L53)) — Click to expand</summary>

     - Status:
     - Description: Resource 219 has been added to the mk.dll file as an image of Attila, which will now be displayed in the diplomacy window when interacting with barbarians/mercenaries. The images for the leaders Louis XIV and Joan of Arc (229 and 250) in the diplomacy window have also been changed to look more like the originals, instead of the previous ones, which were the same for the Greeks and the French.

     </details>

240. <details><summary>duplij-240 (N184) - Feature?: (contains duplij-54 (L54)) — Click to expand</summary>

     - Status:
     - Description: The MsgWaitForMultipleObjectsEx procedure has been implemented in all civ2.exe modification files to reduce the load on the processor core, which adds a 1ms delay while waiting for new messages; The compatibility of the modification with the Civ2UIA patch up to version 1.22.2.3564 has been improved in terms of the first three executable files.

     </details>

241. <details><summary>duplij-241 (N185) - Combo? Fix?/Feature? — Click to expand</summary>

     - Status:
     - Description: Fixed the implementation of MsgWaitForMultipleObjectsEx to reduce CPU load when no messages are present (fixed a bug that caused audio distortion or lag; added thread suspension during idle wait cycles - one such cycle occurred during combat while waiting for explosion animation changes; reworked the message polling procedure for network mode so windows no longer lag or lose connection during multiplayer games); MsgWaitForMultipleObjectsEx has also been implemented in the civ2map.exe file; Menu audio (menuloop and menuend) switched to the PlaySound procedure for high-quality playback (an excellent choice when audio mixing isn't required); other audio fixes (when loading a scenario with a menuloop sound, it now starts playing, when exiting a scenario or multiplayer game, the standard menu sound now plays). Added playback of the unused sound `pompcirc.wav` when the Throne Room window appears during the event.

     </details>

242. <details><summary>duplij-242 (N186) - Feature?: (contains duplij-35 (L35)) — Click to expand</summary>

     - Status:
     - Description: Introduced a parameter in the @Cosmic tag in the rules.txt file that controls the ability to build Nuclear weapons in cities; adjusted variables to account for this parameter when creating scenarios. Added the corresponding line in the @EDITCOSMIC tag in the debug.txt file; The @NOCITY tag has been added to the game.txt file.

     </details>

243. <details><summary>duplij-243 (N187) - Fix?: (contains duplij-55 (L55)) — Click to expand</summary>

     - Status: duplij-55 (L55): fixed on OC2 main; see https://github.com/richardpenman/civ2/issues/24#issuecomment-6817643124; still unresolved: civ.ini read/write in the game folder, Simultaneous/MaxPlayers fields, MusicAndSoundVolume, Load/Scenario dialogs opening in the game folders, multiplayer crash, XDaemon.dll socket buffer
     - Description: Fixed how the game handles the civ.ini file. It now reads from and writes to the game folder instead of the Windows system folder. Fixed several fields that the game was recording incorrectly after reading a specified parameter into them (Simultaneous and MaxPlayers). All existing parameters are included in the file, and a new one, 'MusicAndSoundVolume', has been added, allowing you to set the volume level for music and sounds before starting a new game; When starting the game and selecting the "Load a Game" or "Begin Scenario" options, the window that opens will now point directly to the relevant folders within the game directory, rather than defaulting to the "Documents" folder; An issue causing crashes in multiplayer mode has been fixed; additionally, a patch has been implemented in XDaemon.dll that sets a fixed size for the socket buffer; Fixed a rare bug in a calculation related to the Civilizations array that produced a negative value, causing data in preceding memory locations to be overwritten. This often reset City Improvements names to null or filled them with garbage data, frequently leading to a crash.

     </details>

244. <details><summary>duplij-244 (SET1) - Feature? — Click to expand</summary>

     - Status:
     - Description: Music playback (duplij-2 (L2), duplij-155 (N99))
     - 244.1 duplij-SET1-L2: This executive file contains the ability to listen to tracks from the MUSIC folder. Playable format WAV 44100/22050khz 8/16bit stereo/mono. Tracks are checked for a name in the form TRACK\*\*, so it is advisable to start the initial track with 01 with the possibility of increasing to 99 pieces. Sound playback uses the standard game procedure sndPlaySoundA in asynchronous mode. A new track is played when the previous one ends, provided that a new player turn begins or any left mouse click on the map occurs. To play music, additional RAM is required in the size of the WAV file being played. The number of played tracks is specified in the rules.txt file using the @Cosmic parameter. There are currently 25 tracks in the archive. To use the music playback added in the FoxHead 1.21 patch, instead of this one, set the parameter to 0
     - 244.2 duplij-SET1-N99: The ability to listen to music from the Music folder has been added. The mechanism is implemented on the imported procedure for playing sounds PlaySoundA. Since this is playing sounds, then after playback is complete, the next track does not start itself, so I made a binding for choosing a random track on a new game turn of a human or by left-clicking on a game map. A new track starts only when the previous one ends. You can start a specific track from the Game menu 'Pick Music'. This implementation can play up to 80 tracks in the Music folder. Supported sound format is WAV 44100/22050kHz 8/16bit stereo/mono, called via PlaySoundA procedure with the SND_ASYNC, SND_NODEFAULT, SND_NOSTOP flags. In the Rules.txt file, the @Cosmic tag, an option that specifies how many tracks can be played, has been added. Setting the value to 0 will disable this playback mechanism. Newly added tracks in the form of TRACK\*\* can simply be placed in the Music folder and the @Cosmic tag edited. In addition, you can also add the track name to the Game.txt file, @PICKMUSICGOLD tag, for further manual selection in the menu. Track number TRACK\*\* is equal to the line number.
     - 244.3 Difference: legacy uses sndPlaySoundA with up to 99 tracks, and setting 0 falls back to FoxAhead's music. New uses PlaySoundA with up to 80 tracks and adds a Pick Music menu, and setting 0 disables music entirely.

     </details>

245. <details><summary>duplij-245 (SET2) - Mod? — Click to expand</summary>

     - Status:
     - Description: Grassland resources (duplij-17 (L17), duplij-28 (L28), duplij-110 (N54))
     - 245.1 duplij-SET2-L17: Two resources have been unlocked in the grassland, named Village and Settlement. You can set resources in the rules.txt file. Corrected pictures in the TERRAIN1.GIF file. Corrected the Civilopedia help to account for these resources correctly. Added the ability to display new resources using the map editor Civ2Map.exe
     - 245.2 duplij-SET2-L28: The resource value of grassland has been changed, now by default it does not provide production in @TERRAIN, however, if there is a mark in the form of a shield on grasslands, the cell will receive an additional one production. If the shield coincides with a resource on a grassland tile (Village or Settlement) it will give them one more production, if not, then the production will be the base one, which is indicated for the Village and Settlement in @TERRAIN
     - 245.3 duplij-SET2-N54: Two resources on the Grassland square type have been unlocked, named Cattle and Garden, added corresponding graphic images to the TERRAIN1.GIF file, and a Description in Civilopedia. If the grassland resource square does NOT match the shield (bush) icon, then there will be one less shield production resource in the square.
     - 245.4 Difference: the resource names differ (Village and Settlement vs. Cattle and Garden). The shield rules are described differently in each version.

     </details>

246. <details><summary>duplij-246 (SET3) - Mod? — Click to expand</summary>

     - Status:
     - Description: Harbor, Offshore Platform and Superhighways (duplij-22 (L22), duplij-105 (N49))
     - 246.1 duplij-SET3-L22: Added the ability to build a Harbor and an Oil Platform for cities if the city has access to at least one water cell in the coverage area. If there is not a single suitable area for the Super Highway in the city's coverage area (desert, plain, grassland), it will not be possible to build it
     - 246.2 duplij-SET3-N49: Harbor and Offshore Platform can now be built in a city if it has at least one ocean square in its radius, and vice versa - Superhighways can no longer be built if the city does not have a single ground square suitable for a highway in its radius (desert, plains, grassland, and tundra are suitable).
     - 246.3 Difference: the new version also counts tundra as a suitable square for Superhighways.

     </details>

247. <details><summary>duplij-247 (SET4) - Feature? — Click to expand</summary>

     - Status:
     - Description: Unit window sizes (duplij-32 (L32), duplij-66 (N10), duplij-70 (N14))
     - 247.1 duplij-SET4-L32: The number of displayed units in city support has been increased from 8 to 18 units (three rows of 6 units each). The number of selectable units from one cell has been increased from 9 to 18 units
     - 247.2 duplij-SET4-N10: The window, which is often used in the game when selecting units from one square (stack), when gifting units to another civilization, etc., has been increased from a maximum of 9 to 20 units (16 in civ2hc). The Civ2UIA patch changes the approach to the window and adds a scrollbar to it, and also allows you to scroll it with a mouse wheel.
     - 247.3 duplij-SET4-N14: Increased capacity of units displayed, both in Units Present in the city window and in Units Supported window. The Units Supported window will get a scrollbar if you use the Civ2UIA patch.
     - 247.4 Difference: the limits differ (legacy allows 18 selectable units, new allows 20). The single legacy item matches two separate new items.

     </details>

248. <details><summary>duplij-248 (SET5) - Feature? — Click to expand</summary>

     - Status:
     - Description: City specialists (duplij-33 (L33), duplij-71 (N15))
     - 248.1 duplij-SET5-L33: The maximum size of a city in which all specialists can be managed manually has been increased from 36 to 52. At the same time, the 37th specialist will work together with 21, the 38th in tandem with 22, etc. This had to be done due to limited address space
     - 248.2 duplij-SET5-N15: The display of City Resources has been slightly shifted; it is not so much compressed now. The placement of Citizens at the very top of the city window has been slightly expanded. The next specialist in the city (41st citizen) that exceeds a size of 40, provided that all resources used by this city could not have been used before, will now also be active but will work in tandem with the first specialist (25th citizen); the second specialist (26th citizen) will be in tandem with the 18th (42nd citizen), the 3rd with the 19th, and so on, up to 16x32.
     - 248.3 Difference: the size thresholds and the specialist pairings differ. The new version also changes the city window layout.

     </details>

249. <details><summary>duplij-249 (SET6) - Feature? — Click to expand</summary>

     - Status:
     - Description: Right-click unit window (duplij-41 (L41), duplij-101 (N45))
     - 249.1 duplij-SET6-L41: Added an optional unit window by right-clicking on the map, instead of the previous right-click handler. This processing occurs according to the cell coordinate, if there is a unit in the cell, a procedure with different options is called, while the unit itself does not immediately become active. If there is no unit, it simply moves around the map. Several options have been added to this window in addition to the usual ones, this is sending a unit to the home city (flying units use a game exploit that bypasses the check for flights and fuel if you do not make the last move, so they can be sent without thinking whether they will fly or not), moving a unit in a group to the very top. In the open map (cheat) mode, through this menu you can heal damaged units, remove the assignment of units to the city, create units, send units without a limit on moves and reactivate already departed units by resetting moves (folder Right_mouse_click saved separately)
     - 249.2 duplij-SET6-N45: An optional window for a unit by right-clicking on the map has been added instead of the previous right-click handler. This processing occurs based on the square coordinate. If there is a unit in the square, a procedure with different options is called, while the unit itself does not immediately become active. If there is no unit, it simply moves the cursor around the map. Return the previous right-click processing here E8 78 26 07 00 replace with E8 16 09 FF FF. Several options have been added to this window in addition to the usual ones, these are 'Send Unit to the Home City' (it is better not to send air units unless you activate option from paragraph 46, since there is no check for range and the unit can be lost), 'Conduct Training' - training will take place, which will allow NON-veteran to receive this status at some random rate (with each turn, the success probability increases), but in any case, after clicking, the unit will lose all moves, if the training is successful, a corresponding message will pop up. When the whole map is revealed in cheat mode, through this menu, you can heal damaged units, remove units' home cities, create units, send units without limitation on moves, and reactivate already departed units by resetting moves. To remove the restrictions in this mode on unlimited moves for AI, you need to change 85 C3 74 5B, where 5B is changed to 48. To view the map as before, you can still press the 'v' key.
     - 249.3 Difference: legacy offers "move unit to top of group". New offers "Conduct Training" instead. Sending air units home works differently (legacy relies on the fuel exploit; new warns that air units can be lost).

     </details>

250. <details><summary>duplij-250 (SET7) - Feature? — Click to expand</summary>

     - Status:
     - Description: Deleting your own cities (duplij-45 (L45), duplij-203 (N147))
     - 250.1 duplij-SET7-L45: The ability to delete your cities in the normal game mode has been activated (by default, this ability became active after activating the cheat mode). Deleting a city occurs when moving the active frame to the target city and pressing the Shift-D keys. If there are units in the city, you will first be prompted to delete them and only then the city
     - 250.2 duplij-SET7-N147: The check that prohibits deleting your city, if cheat mode is not active, has been removed. Now, an empty city can be deleted like a unit, using a key combination (Shift+D), but this will affect the reputation if deletion occurs when the cheat mode is not active. The size of the destroyed city divided by 8 will give a value that will be added to the reputation, worsening it.
     - 250.3 Difference: legacy prompts you to delete the units first. New allows only empty cities and adds a reputation penalty.

     </details>

251. <details><summary>duplij-251 (SET8) - Fix? — Click to expand</summary>

     - Status:
     - Description: Clicking and double-clicking unexplored squares (duplij-20 (L20), duplij-160 (N104))
     - 251.1 duplij-SET8-L20: Now, when you click on a cell that is not open by the player, if there is a unit or city of another player there, the map will still be centered. In open map mode, double-clicking on any open cell that was previously not visible will bring up the terrain information panel. In normal map mode, if a city or a troop stands on a previously opened area of the map, but they are not visible to the player, double-clicking on the cell will bring up the landscape information panel
     - 251.2 duplij-SET8-N104: A display of information about a square by double-clicking the left mouse button has been corrected. Previously, if a square was opened by a player and there was a hidden unit or city of another player on it, then the window with information about the square did not pop up, and thus this indirectly indicated the presence of a unit or city there. Now, information about the square will be shown in any case; the exception is if there is a unit or city of the player himself in the square. In addition, information about the squares will also be displayed in the open map mode, on those squares that are visible at the time of activity of this mode, but were invisible before.
     - 251.3 Difference: the double-click behavior matches. Legacy also centers the map when you click an unexplored square.

     </details>

252. <details><summary>duplij-252 (SET9) - Mod? — Click to expand</summary>

     - Status:
     - Description: Diplomacy backgrounds and leader portraits (duplij-29 (L29), duplij-30 (L30), duplij-123 (N67), duplij-188 (N132))
     - 252.1 duplij-SET9-L29: The diplomatic background has been corrected when there is contact with another race that has a government type of communism or anarchy (file mk.dll)
     - 252.2 duplij-SET9-L30: Added alternative terrain and units to the ALT folder, as well as an option using portraits of leaders Stalin and Hitler when playing for the USSR or Germany (may be useful in World War II scenarios). These options are optional, if you want to use them, copy the contents of the folders to the game folder or ignore it
     - 252.3 duplij-SET9-N67: The background of the communist and anarchist government in diplomacy has been changed; now, next to the portrait of the leader will be a star or a symbol of anarchy, instead of the previous icon of monarchy. Portrait of Frederick of the Germans has been replaced with Hitler (resource images 200, 203, 222 in the mk.dll file have been replaced with the eXeScope program).
     - 252.4 duplij-SET9-N132: A portrait of Stalin has been added instead of Lenin (file mk.dll). The leader's initials have been changed in the Rules.txt file, @leaders tag.
     - 252.5 Difference: L29 matches the first half of N67. L30's portraits are optional extras in the ALT folder, while N67 (Hitler) and N132 (Stalin) build them into the game.

     </details>

253. <details><summary>duplij-253 (SET10) - Fix? — Click to expand</summary>

     - Status: duplij-51 (L51): OC2 code appears not affected, would need more details or a recreate or a .SAV file to dig deeper; see https://github.com/richardpenman/civ2/issues/24#issuecomment-6817643124; still unresolved: the N173 side of the comparison, so the set isn't confirmed
     - Description: Unit image in an upgrade info window, possible match (duplij-51 (L51), duplij-229 (N173))
     - 253.1 duplij-SET10-L51: Fixed a unit image where, when exchanging technologies in Diplomacy and the player received a new technology (if the player has a wonder of the world, Leonardo's Workshop), a window could pop up about upgrading a certain unit and a distorted image of the unit could appear in it
     - 253.2 duplij-SET10-N173: Fixed the issue with technology where unlocking Tactics should grant Cavalry units in huts. Corrected the information window display to show the unit icon properly after unlocking Explosives with an Engineer, or Robotics with a Converter. Empty barbarian ships are now deleted immediately to prevent repeated landing messages when they linger near a city. Added the option to declare war through diplomacy, provided the player is not under a Republic or Democracy government, and the opponent does not have the Great Wall or United Nations wonders. Alternatively, if the player controls the United Nations, war can also be initiated with Senate approval, regardless of government type. The AI may attempt to appease you by offering tribute, such as gold or technology. You can choose to accept this offer or reject it by declaring war. The @DIPLOMACYMENU tag has been updated with a new entry, and the tags @THREATWAR(2) and @THRWAR(0,1,2) have been added to the Game.txt file; the @ALMOSTWONDER message has been updated to accurately reflect the production bonuses the AI receives.
     - 253.3 Difference: both fix a unit image in an upgrade info window, but in different situations: legacy during a diplomacy tech trade with Leonardo's Workshop, new after unlocking Explosives or Robotics. This may not be the same fix.

     </details>

