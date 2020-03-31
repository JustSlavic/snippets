# Задание 1: Экосистема

### Условие задачи:

Для изучения экосистемы в районе озера Лиманчик, принято решение использовать *GPS*-датчики, прикрепленные к стрекозам, аистам, лягушкам и ежам. 
Но прежде чем приступать к полевым испытаниям, необходимо провести моделирование.
Ваша задача промоделировать поведение экосистемы.
Объекты модели: стрекозы, аисты, лягушки, ежи. 
Лягушки и ежи двигаются по плоскости, стрекозы и аисты в трехмерном пространстве зоны моделирования, представляющей собой куб с диагональю, соединяющей точки `(0, 0, 0)` и `(10^6, 10^6, 10^6)`. 
Каждый объект модели имеет целочисленные координаты и целочисленный вектор скорости в любой момент времени. 
Время дискретно. 
Процесс моделирования одной итерации жизни модели состоит из трех этапов: перемещение объектов, анализ событий, обработка событий. 

Прежде чем перейти к описанию этих этапов, нужно пояснить некоторые особенности моделирования. 
Ежи едят лягушек, лягушки – стрекоз, аисты едят и стрекоз, и лягушек. 
Если объект *А* может съесть объект *Б*, то объект *Б* боится объект *А* и будет стараться избежать встречи с ним. 
Каждый объект обладает способностью чувствовать другие объекты на некотором расстоянии. 
Все объекты кроме стрекоз имеют способность съедать, причем ежи и аисты на расстоянии `1`, а лягушки на расстоянии не более `3`.
Если объект может быть съеден и сам может съесть кого-то, то он в выборе направления
перемещения ведет себя как жертва. 
Т.е. инстинкт самосохранения сильнее!

##### Поведение хищника
Если объект *А* почувствовал объект *Б* и при этом может его съесть, то хищник постарается приблизиться к жертве, причем при появлении другого съедобного объекта хищник цель не меняет. 
При выходе жертвы из зоны чувствительности хищника, последний прекращает преследование и может выбрать себе другую цель. 
Если хищник сократил расстояние с жертвой до расстояния доступного для использования способности съедения, то жертва помечается на удаление на этапе анализа и будет удалены на этапе обработки событий.
Если жертв в зоне чувствительности хищника нет, то хищник продолжает двигаться в прежнем направлении.
Исключение составляет, ситуация соприкосновения с землей.

##### Поведение жертвы
Если хищников в зоне чувствительности жертвы нет, то жертва продолжает двигаться в прежнем направлении. 
Если объект *Б* почувствовал объект *А*, который может съесть *Б*, то жертва постарается удалиться от *А*. 
Если хищников в зоне чувствительности жертвы много, то жертва будет пытаться удалиться от самого ближнего из хищников. 
Термин «удалиться» в данном контексте означает двигаться в направлении противоположном к хищнику. 
Исключение составляет, ситуация соприкосновения с землей. 

##### Соприкосновение с землей
Плоскость `z = 0` представляет собой землю. 

Если объект после перемещения оказывается под землей `z < 0`, то его координаты должны быть пересчитаны по следующему правилу: `x = x0 – vx*z0/vz`, `y = y0 – vy*z0/vz`, `z = 0`, где `(x, y, z)` – новое положение объекта, `(x0, y0, z0)` – предыдущее положение объекта, `(vx, vy, vz)` – вектор скоростиобъекта. 
При этом операция деления должна быть целочисленной и выполняться после операции умножения.
Пусть объект *А* находится на земле. 
Пусть для объекта *А* возникло событие изменения направления движения с `vz ≠ 0`.
Если он может летать и `vz > 0`, то этот объект двигается по обычным правилам.
В противном случае, вектор скорости нужно пересчитать с учетом проекции на плоскость `z = 0`. 
Формула: `vnew = µ vproj`, `µ = |v0|/|vproj|`, где `v0` – вектор начальной скорости объекта *А*, `vproj` – проекция на плоскость `z = 0` расчетного вектора скорости.
Формула для вычисления расчетного вектора скорости предполагает вычисления в рамках вещественных чисел (*double*).
В трехмерном случае эта формула имеет вид: `v = λ*d`, `d = r – p`, `λ = |v0|/|d|`, где `r` – позиция жертвы в пространстве, `p` – позиция хищника в пространстве, `v0` – вектор начальной скорости объекта для, которого производится вычисление.
Полученный вектор, при соблюдении описанных выше условий должен быть преобразован в соответствии с формулой, учитывающей положение объекта на земле.
В завершении каждая из координат вектора должна быть округлена до меньшего ближайшего целого. 

##### Этап перемещения 
На этом этапе моделирования одной итерации, все объекты смещаются в направлении определенном на предыдущей итерации. 

##### Этап анализа
На этом этапе моделирования одной итерации, все объекты анализируются на события.
События могут быть трех видов: удаление объекта, изменение направления движения, остановка моделирования.
Событие удаления объекта генерируется в случае, если объект съеден или если он вышел за границу области моделирования, и при этом он не убегает ни от кого и не догоняет никого.
Событие изменения направления движение возникает в ситуациях, описанных в поведении хищника и жертвы.
Событие остановки моделирования возникает в случае истечения заданного количества итераций или в случае выхода за границы зоны моделирования объекта, который убегает от хищника или преследует жертву.
При этом соприкосновение с землей не вызывает остановку моделирования, но может вызывать смену направления движения. 

##### Этап обработки
Каждое событие обрабатывается в соответствии со своим наименованием.

### Формат входного файла

В первой строке, приведены девять чисел, разделенных одиночными пробелами: `D`, `DS`, `S`, `SS`, `F`, `FS`, `H`, `HS`, `N` `(0 ≤ D, S, F, H ≤ 100, 0 ≤ HS, DS, SS, FS ≤15, 1 ≤ N ≤10000)`.
Числа `D` и `DS` означают количество стрекоз и их дальность чувствительности, `S` – количество аистов и их дальность чувствительности, `F` – количество лягушек и их дальность чувствительности, `H` – количество ежей и их дальность чувствительности, `N` – количество итераций модели.
В каждой следующей строке описан ровно один объект модели в следующем формате `x`, `y`, `z`, `vx`, `vy`, `vz` для стрекоз и аистов, и в формате `x`, `y`, `vx`, `vy` для лягушек и ежей. 
Все числа разделены одиночными пробелами, и координаты каждого объекта гарантированно принадлежат зоне моделирования. Числа `x`, `y` и `z` определяют начальные координаты объекта, числа `vx`, `vy`, и `vz` определяют начальную скорость. Первые `D` объектов являются стрекозами, далее описаны аисты, затем лягушки и последними ежи.

### Формат выходного файла

В формате, описанном в пункте «Формат входного файла», необходимо описать состояние системы при завершении моделирования. 
Число `N` в этом случае описывает последнюю смоделированную итерацию.

### Требования
1. Описать иерархию классов «Объекты моделирования».
2. Описать иерархию классов «События».
3. Чтение и запись в файл должно быть отделено от модели.

Лектор: Штейнберг Роман Борисович.

Осень 2016, ЮФУ.