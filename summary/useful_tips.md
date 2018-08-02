# Useful tips


## 2018.7.25
English:
0.  Preceding zeros may be omitted
1. This convention might be employed. 
2. unicode combining character
3. an 'a' character followed by a combining ' ¨ ' character.
4. costom定制


Tec:
4. [How input unicode?](https://en.wikipedia.org/wiki/Unicode_input)
  4.1 an applet from which one can select the character
  4.2 pasting from the operating system's clipboard
  4.3 by typing a certain sequence of keys on a physical keyboard
    4.3.1 windows alt+十进制编码，如alt+93,是]，u+005d
    4.3.2 html的unitcode表示：&开始，#代表后面带的是数字（十进制，16进制都可，16禁止前面加x），有些也可以加上unicode的名字，最后;结尾。如：
    ```
      // 这个字©的不同表示方式
      1. &#169; (decimal input)
      2. &#xa9; (hexadecimal input)
      3. &copy; (entity name) 

    ```
    4.3.3 在windows上面的16进制输入
      1. 注册表HKEY_CURRENT_USER\Control Panel\Input Method 中添加名为EnableHexNumpad的string type (REG_SZ) value，并设置成1，重启。就可以用alt + ‘+’ + 16进制编码了。但好像没用。。。

    4.3.4 在linux上输入，可以哦。\o/
      1. Hold Ctrl+⇧ Shift and type u followed by the hex digits. Then release Ctrl+⇧ Shift.
      2. Enter Ctrl+⇧ Shift+u, release, then type the hex digits, and press ↵ Enter (or Space or even on some systems, press and release ⇧ Shift or Ctrl ).

     
      3. test: ͡o͜o͡ 333  [https://www.lennyfaces.net/](https://www.lennyfaces.net/) interesting faces.




2. line breaker
一个都不能少！






