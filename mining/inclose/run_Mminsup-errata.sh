max=50
for i in `seq 1 $max`
do

	./InClose5 ../datasets/new_adult_discretized_asc ../run_adult/minrows_adult10 1 Result/new_adultOutput10.m ../datasets/new_adult_labels 0.1 0.0 1 0 1 0

	./InClose5 ../datasets/breast_asc ../run_breast/minrows_breast01 1 Result/breastOutput01.m ../datasets/breast_labels 0.01 0.0 1 0 1 0

	./InClose5 ../datasets/breast_asc ../run_breast/minrows_breast07 1 Result/breastOutput07.m ../datasets/breast_labels 0.07 0.0 1 0 1 0

	./InClose5 ../datasets/breast_asc ../run_breast/minrows_breast09 1 Result/breastOutput09.m ../datasets/breast_labels 0.09 0.0 1 0 1 0

	./InClose5 ../datasets/german_asc ../run_german/minrows_german06 1 Result/germanOutput06.m ../datasets/german_labels 0.06 0.0 1 0 1 0

	./InClose5 ../datasets/new_internet_discretized_asc ../run_internet/minrows_internet03 1 Result/new_internetOutput03.m ../datasets/new_internet_labels 0.03 0.0 1 0 1 0

	./InClose5 ../datasets/new_internet_discretized_asc ../run_internet/minrows_internet07 1 Result/new_internetOutput07.m ../datasets/new_internet_labels 0.07 0.0 1 0 1 0

	./InClose5 ../datasets/new_mushroom_discretized_asc ../run_mush/minrows_mush07 1 Result/new_mushroomOutput07.m ../datasets/new_mushroom_labels 0.07 0.0 1 0 1 0

	./InClose5 ../datasets/new_mushroom_discretized_asc ../run_mush/minrows_mush08 1 Result/new_mushroomOutput08.m ../datasets/new_mushroom_labels 0.08 0.0 1 0 1 0

	./InClose5 ../datasets/new_mushroom_discretized_asc ../run_mush/minrows_mush10 1 Result/new_mushroomOutput10.m ../datasets/new_mushroom_labels 0.1 0.0 1 0 1 0
	./../inclose5-ZDC/InClose5 ../datasets/german_asc 25 1 Result/germanOutput06.m ../datasets/german_labels 0.06 0
done
