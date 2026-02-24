using System;
using System.Collections.Generic;

namespace CoffeeShop.BadApproach
{
    public enum MilkType
    {
        Нет,
        Коровье,
        Овсяное,
        Миндальное,
        Кокосовое,
        Банановое,
        Соевое
    }
    public enum SyrupType
    {
        Нет,
        Карамель,
        Ваниль,
        Фундук,
        Шоколад,
        Кокос,
        Лаванда,
        Мятный
    }
    public enum Temperature
    {
        Горячий,
        Теплый,
        Холодный,
        Со_Льдом
    }
    public enum Additive
    {
        ВзбитыеСливки,
        Мороженое,
        Маршмеллоу
    }
    public enum Topping
    {
        Нет,
        Шоколадный,
        Карамельный,
        Ягодный
    }
    public enum Decor
    {
        Нет,
        Корица,
        Какао,
        ШоколаднаяСтружка,
        ЛаттеАрт
    }
    public enum Packaging
    {
        ВЗаведении,
        ССобой,
        СвояКружка
    }

    // базовый абстрактный класс
    public abstract class CoffeeDrink
    {
        public Temperature Temperature { get; set; }
        public int Volume { get; set; }
        public Packaging Packaging { get; set; }
        public List<Additive> Additives { get; private set; }
        public Topping Topping { get; set; }
        public Decor Decor { get; set; }
        public CoffeeDrink()
        {
            Additives = new List<Additive>();
            Temperature = Temperature.Горячий;
            Volume = 300;
            Packaging = Packaging.ВЗаведении;
            Topping = Topping.Нет;
            Decor = Decor.Нет;
        }
        public void AddAdditive(Additive additive)
        {
            Additives.Add(additive);
        }
        public abstract string GetDescription();
        protected abstract void Validate();
    }

    public class Espresso : CoffeeDrink
    {
        public bool DoubleShot { get; set; }

        public Espresso(bool doubleShot = false)
        {
            DoubleShot = doubleShot;
            Volume = doubleShot ? 60 : 30;
            Validate();
        }

        protected override void Validate()
        {
            if (Additives.Count > 0)
                throw new Exception("Эспрессо не подают с добавками!");
        }

        public override string GetDescription()
        {
            string result = "Эспрессо, ";
            result += DoubleShot ? "двойной" : "одинарный";
            result += $", {Volume}мл";
            return result;
        }
    }

    public class Cappuccino : CoffeeDrink
    {
        public MilkType Milk { get; set; }
        public SyrupType Syrup { get; set; }
        public double SyrupDose { get; set; }
        public bool ExtraFoam { get; set; }

        public Cappuccino(
            MilkType milk = MilkType.Коровье,
            SyrupType syrup = SyrupType.Нет,
            double syrupDose = 1,
            bool extraFoam = false)
        {
            Milk = milk;
            Syrup = syrup;
            SyrupDose = syrupDose;
            ExtraFoam = extraFoam;
            Validate();
        }

        protected override void Validate()
        {
            if (Milk == MilkType.Нет)
                throw new Exception("Капучино обязательно с молоком!");
        }

        public override string GetDescription()
        {
            string result = $"Капучино на {GetMilkName()} молоке";

            if (ExtraFoam)
                result += " с дополнительной пенкой";

            if (Syrup != SyrupType.Нет)
                result += $", сироп {Syrup} ({SyrupDose} дозы)";

            result += $", {Volume}мл";
            return result;
        }
        private string GetMilkName()
        {
            switch (Milk)
            {
                case MilkType.Коровье: return "коровьем";
                case MilkType.Овсяное: return "овсяном";
                case MilkType.Миндальное: return "миндальном";
                case MilkType.Кокосовое: return "кокосовом";
                case MilkType.Банановое: return "банановом";
                case MilkType.Соевое: return "соевом";
                default: return "???";
            }
        }
    }
    public class Latte : CoffeeDrink
    {
        public MilkType Milk { get; set; }
        public SyrupType Syrup { get; set; }
        public double SyrupDose { get; set; }
        public bool LatteArt { get; set; }

        public Latte(
            MilkType milk = MilkType.Коровье,
            SyrupType syrup = SyrupType.Нет,
            double syrupDose = 1,
            bool latteArt = false)
        {
            Milk = milk;
            Syrup = syrup;
            SyrupDose = syrupDose;
            LatteArt = latteArt;

            if (LatteArt)
                Decor = Decor.ЛаттеАрт;

            Validate();
        }
        protected override void Validate()
        {
            if (Milk == MilkType.Нет)
                throw new Exception("Латте обязательно с молоком!");
        }

        public override string GetDescription()
        {
            string result = $"Латте на {GetMilkName()} молоке";
            if (Syrup != SyrupType.Нет)
                result += $", сироп {Syrup} ({SyrupDose} дозы)";
            result += $", {Volume}мл";
            if (LatteArt)
                result += ", с латте-артом";
            return result;
        }
        private string GetMilkName()
        {
            switch (Milk)
            {
                case MilkType.Коровье: return "коровьем";
                case MilkType.Овсяное: return "овсяном";
                case MilkType.Миндальное: return "миндальном";
                case MilkType.Кокосовое: return "кокосовом";
                case MilkType.Банановое: return "банановом";
                case MilkType.Соевое: return "соевом";
                default: return "???";
            }
        }
    }
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("пример созданных напитков\n");

            var espresso = new Espresso(false);
            Console.WriteLine($"1. {espresso.GetDescription()}");

            var cappuccino = new Cappuccino(MilkType.Овсяное, SyrupType.Карамель, 2, true);
            Console.WriteLine($"2. {cappuccino.GetDescription()}");

            var latte = new Latte(MilkType.Миндальное, SyrupType.Ваниль, 1, true);
            Console.WriteLine($"3. {latte.GetDescription()}");
        }
    }
}
