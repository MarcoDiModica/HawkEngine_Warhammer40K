using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Numerics;

namespace HawkEngine
{
    public static class PhysicsBindings
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern GameObject[] OverlapSphere(Vector3 position, float radius, string tag);
    }

    public class Pathfinding
    {
        private readonly int width = 20;
        private readonly int height = 20;
        private readonly float cellSize = 1f;
        private readonly Node[,] grid;

        public Pathfinding()
        {
            grid = new Node[width, height];
            for (int x = 0; x < width; x++)
                for (int y = 0; y < height; y++)
                {
                    var worldPos = new Vector3(x + 0.5f, 0.5f, y + 0.5f);
                    var hits = PhysicsBindings.OverlapSphere(worldPos, cellSize * 0.45f, "Obstacle");
                    bool walkable = hits == null || hits.Length == 0;
                    grid[x, y] = new Node(x, y, walkable);
                }
        }

        public List<Vector3> FindPath(Vector3 startWorld, Vector3 targetWorld)
        {
            var start = NodeFromWorldPoint(startWorld);
            var target = NodeFromWorldPoint(targetWorld);
            if (!start.Walkable || !target.Walkable) return null;

            var openSet = new List<Node> { start };
            var closedSet = new HashSet<Node>();
            start.GCost = 0;
            start.HCost = Heuristic(start, target);

            while (openSet.Count > 0)
            {
                openSet.Sort((a, b) =>
                {
                    int cmp = a.FCost.CompareTo(b.FCost);
                    return (cmp != 0) ? cmp : a.HCost.CompareTo(b.HCost);
                });
                var current = openSet[0];
                openSet.RemoveAt(0);
                closedSet.Add(current);

                if (current == target)
                    return RetracePath(start, target);

                foreach (var neighbor in GetNeighbors(current))
                {
                    if (!neighbor.Walkable || closedSet.Contains(neighbor))
                        continue;

                    float cost = current.GCost + Distance(current, neighbor);
                    if (cost < neighbor.GCost || !openSet.Contains(neighbor))
                    {
                        neighbor.GCost = cost;
                        neighbor.HCost = Heuristic(neighbor, target);
                        neighbor.Parent = current;
                        if (!openSet.Contains(neighbor))
                            openSet.Add(neighbor);
                    }
                }
            }

            return null;
        }

        private List<Vector3> RetracePath(Node start, Node end)
        {
            var path = new List<Vector3>();
            var curr = end;
            while (curr != start)
            {
                path.Add(WorldPoint(curr));
                curr = curr.Parent;
            }
            path.Add(WorldPoint(start));
            path.Reverse();
            return path;
        }

        private Node NodeFromWorldPoint(Vector3 world)
        {
            int x = Math.Max(0, Math.Min(width - 1, (int)world.X));
            int y = Math.Max(0, Math.Min(height - 1, (int)world.Z));
            return grid[x, y];
        }

        private Vector3 WorldPoint(Node n)
            => new Vector3(n.X + 0.5f, 0.5f, n.Y + 0.5f);

        private IEnumerable<Node> GetNeighbors(Node node)
        {
            for (int dx = -1; dx <= 1; dx++)
                for (int dy = -1; dy <= 1; dy++)
                {
                    if (dx == 0 && dy == 0) continue;
                    int nx = node.X + dx, ny = node.Y + dy;
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                    // prevent corner‐cutting
                    if (dx != 0 && dy != 0)
                    {
                        if (!grid[node.X + dx, node.Y].Walkable ||
                            !grid[node.X, node.Y + dy].Walkable)
                            continue;
                    }
                    yield return grid[nx, ny];
                }
        }

        private static readonly float DiagonalCost = (float)Math.Sqrt(2);
        private float Distance(Node a, Node b) =>
            (a.X != b.X && a.Y != b.Y) ? DiagonalCost : 1f;
        private float Heuristic(Node a, Node b) =>
            Math.Max(Math.Abs(a.X - b.X), Math.Abs(a.Y - b.Y));

        private class Node
        {
            public int X, Y;
            public bool Walkable;
            public float GCost = float.MaxValue, HCost;
            public Node Parent;
            public float FCost => GCost + HCost;
            public Node(int x, int y, bool walkable)
            {
                X = x; Y = y; Walkable = walkable;
            }
        }
    }
}
